#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#endif
#include "Config.h"
#include "MarketDataHandler.h"
#include "Strategy.h"
#include "RiskManager.h"
#include "BrokerSimulator.h"
#include "TradeLogger.h"

using namespace TradingBot;

// Unified trading engine class
class TradingEngine {
public:
    TradingEngine(Config& config, MarketDataHandler& data_handler,
                 Strategy& strategy, RiskManager& risk_manager,
                 BrokerSimulator& broker, TradeLogger& logger)
        : config_(config), data_handler_(data_handler), strategy_(strategy),
          risk_manager_(risk_manager), broker_(broker), logger_(logger) {}
    
    void processTick(const OHLCV& bar, size_t index, 
                    const std::vector<OHLCV>& all_data) {
        std::string reason;
        Signal signal = strategy_.generateSignal(all_data, index, reason);
        
        const std::string& symbol = config_.getSymbol();
        
        // Check for exit conditions if we have a position
        if (broker_.hasPosition(symbol)) {
            Position pos = broker_.getPosition(symbol);
            std::string exit_reason;
            
            if (strategy_.shouldExitPosition(pos.entry_price, bar.close,
                                            config_.getStopLossPercentage(),
                                            config_.getTakeProfitPercentage(),
                                            exit_reason)) {
                // Exit due to stop loss or take profit
                if (broker_.executeSell(symbol, pos.quantity, bar.close, 
                                       bar.date, exit_reason)) {
                    logger_.logTrade(broker_.getLastTrade());
                }
                return;  // Don't process other signals after exit
            }
        }
        
        // Process trading signals
        if (signal == Signal::BUY && !broker_.hasPosition(symbol)) {
            // Calculate position size
            int quantity = risk_manager_.calculatePositionSize(
                bar.close, broker_.getBalance());
            
            if (quantity > 0) {
                std::string risk_reason;
                double position_value = quantity * bar.close;
                
                if (risk_manager_.isTradeAllowed(position_value, 
                                                broker_.getBalance(), 
                                                risk_reason)) {
                    if (broker_.executeBuy(symbol, quantity, bar.close, 
                                          bar.date, reason)) {
                        logger_.logTrade(broker_.getLastTrade());
                        equity_curve_.push_back(broker_.getTotalEquity(symbol, bar.close));
                    }
                } else {
                    std::cout << "[" << bar.date << "] BUY signal ignored: " 
                              << risk_reason << std::endl;
                }
            } else {
                std::cout << "[" << bar.date << "] BUY signal ignored: "
                          << "Insufficient balance for minimum position" << std::endl;
            }
        } 
        else if (signal == Signal::SELL && broker_.hasPosition(symbol)) {
            Position pos = broker_.getPosition(symbol);
            if (broker_.executeSell(symbol, pos.quantity, bar.close, 
                                   bar.date, reason)) {
                logger_.logTrade(broker_.getLastTrade());
                equity_curve_.push_back(broker_.getTotalEquity(symbol, bar.close));
            }
        }
        else {
            // HOLD - record equity if we have position
            if (broker_.hasPosition(symbol)) {
                equity_curve_.push_back(broker_.getTotalEquity(symbol, bar.close));
            }
        }
    }
    
    void closeAllPositions(const OHLCV& final_bar) {
        const std::string& symbol = config_.getSymbol();
        if (broker_.hasPosition(symbol)) {
            Position pos = broker_.getPosition(symbol);
            std::cout << "\nClosing remaining position at market close...\n";
            broker_.executeSell(symbol, pos.quantity, final_bar.close, 
                              final_bar.date, "Market close");
            logger_.logTrade(broker_.getLastTrade());
        }
    }
    
    const std::vector<double>& getEquityCurve() const { return equity_curve_; }
    
private:
    Config& config_;
    MarketDataHandler& data_handler_;
    Strategy& strategy_;
    RiskManager& risk_manager_;
    BrokerSimulator& broker_;
    TradeLogger& logger_;
    std::vector<double> equity_curve_;
};

// Function to run backtest mode
void runBacktest(Config& config) {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "          NIFTY 50 TRADING BOT - BACKTEST MODE\n";
    std::cout << std::string(70, '=') << "\n";
    
    config.printConfig();
    
    // Initialize components
    MarketDataHandler dataHandler;
    if (!dataHandler.loadFromCSV(config.getDataFilePath())) {
        std::cerr << "Failed to load market data. Exiting.\n";
        return;
    }
    
    Strategy strategy(config.getShortMA(), config.getLongMA(), config.getRSIPeriod());
    RiskManager riskManager(config.getRiskPercentage(), config.getMaxExposure());
    BrokerSimulator broker(config.getInitialBalance());
    TradeLogger logger("logs/trades.csv");
    
    TradingEngine engine(config, dataHandler, strategy, riskManager, broker, logger);
    
    // Run backtest
    const auto& all_data = dataHandler.getAllData();
    size_t total_bars = all_data.size();
    size_t start_index = config.getLongMA();
    
    std::cout << "Starting backtest from index " << start_index 
              << " to " << total_bars << "...\n\n";
    
    for (size_t i = start_index; i < total_bars; ++i) {
        const OHLCV& current_bar = all_data[i];
        
        engine.processTick(current_bar, i, all_data);
        
        // Show progress every 10%
        if ((i - start_index) % ((total_bars - start_index) / 10) == 0) {
            double progress = (static_cast<double>(i - start_index) / 
                             (total_bars - start_index)) * 100.0;
            double portfolio_value = broker.getPortfolioValue(
                config.getSymbol(), current_bar.close);
            
            std::cout << "\nProgress: " << static_cast<int>(progress) << "% | "
                      << "Date: " << current_bar.date << " | "
                      << "Price: INR " << current_bar.close << " | "
                      << "Portfolio: INR " << portfolio_value;
            
            if (broker.hasPosition(config.getSymbol())) {
                double unrealized_pl = broker.getUnrealizedPL(
                    config.getSymbol(), current_bar.close);
                std::cout << " | Unrealized P/L: INR " << unrealized_pl;
            }
            std::cout << std::endl;
        }
    }
    
    // Close any remaining positions
    engine.closeAllPositions(all_data.back());
    
    // Calculate and display final statistics
    double final_balance = broker.getBalance();
    double return_pct = ((final_balance - config.getInitialBalance()) / 
                        config.getInitialBalance()) * 100.0;
    
    std::cout << "\n" << std::string(70, '-') << "\n";
    std::cout << "Final Balance:        INR " << final_balance << "\n";
    std::cout << "Total Return:         " << return_pct << "%\n";
    std::cout << std::string(70, '-') << "\n";
    
    Statistics stats = logger.calculateStatistics(
        config.getInitialBalance(), final_balance, engine.getEquityCurve());
    logger.printSummary(stats);
}

// Function to run live simulation mode
void runLiveSimulation(Config& config) {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "        NIFTY 50 TRADING BOT - LIVE SIMULATION MODE\n";
    std::cout << std::string(70, '=') << "\n";
    
    config.printConfig();
    
    MarketDataHandler dataHandler;
    if (!dataHandler.loadFromCSV(config.getDataFilePath())) {
        std::cerr << "Failed to load market data. Exiting.\n";
        return;
    }
    
    Strategy strategy(config.getShortMA(), config.getLongMA(), config.getRSIPeriod());
    RiskManager riskManager(config.getRiskPercentage(), config.getMaxExposure());
    BrokerSimulator broker(config.getInitialBalance());
    TradeLogger logger("logs/live_trades.csv");
    
    TradingEngine engine(config, dataHandler, strategy, riskManager, broker, logger);
    
    std::cout << "Starting live simulation...\n";
    std::cout << "Press Ctrl+C to stop.\n\n";
    
    // Use historical data as initial history
    std::vector<OHLCV> live_data;
    const auto& historical_data = dataHandler.getAllData();
    
    size_t history_size = static_cast<size_t>(config.getLiveHistorySize());
    size_t start_index = historical_data.size() > history_size ? 
                         historical_data.size() - history_size : 0;
    
    for (size_t i = start_index; i < historical_data.size(); ++i) {
        live_data.push_back(historical_data[i]);
    }
    
    std::cout << "Initialized with " << live_data.size() 
              << " historical data points\n\n";
    
    // Simulate live trading
    int iterations = config.getLiveIterations();
    for (int iteration = 0; iteration < iterations; ++iteration) {
        // Generate new "live" data point
        OHLCV new_data = dataHandler.generateLiveData(live_data.back());
        new_data.date = "Live-" + std::to_string(iteration + 1);
        live_data.push_back(new_data);
        
        std::cout << "\n" << std::string(70, '-') << "\n";
        std::cout << "Iteration " << (iteration + 1) << " / " << iterations << "\n";
        std::cout << "Price: INR " << new_data.close 
                  << " (O:" << new_data.open 
                  << " H:" << new_data.high 
                  << " L:" << new_data.low << ")\n";
        
        engine.processTick(new_data, live_data.size() - 1, live_data);
        
        double portfolio_value = broker.getPortfolioValue(
            config.getSymbol(), new_data.close);
        std::cout << "Portfolio Value: INR " << portfolio_value << "\n";
        
        if (broker.hasPosition(config.getSymbol())) {
            Position pos = broker.getPosition(config.getSymbol());
            double unrealized_pl = broker.getUnrealizedPL(
                config.getSymbol(), new_data.close);
            double unrealized_pl_pct = (unrealized_pl / (pos.quantity * pos.entry_price)) * 100.0;
            std::cout << "Position: " << pos.quantity << " shares @ INR " 
                      << pos.entry_price << " | Unrealized P/L: INR " 
                      << unrealized_pl << " (" << unrealized_pl_pct << "%)\n";
        }
        
        // Simulate real-time delay
#ifdef _WIN32
        ::Sleep(config.getLiveDelayMs());
#else
        std::this_thread::sleep_for(std::chrono::milliseconds(config.getLiveDelayMs()));
#endif
    }
    
    // Close any remaining positions
    if (!live_data.empty()) {
        engine.closeAllPositions(live_data.back());
    }
    
    // Final summary
    double final_balance = broker.getBalance();
    double return_pct = ((final_balance - config.getInitialBalance()) / 
                        config.getInitialBalance()) * 100.0;
    
    std::cout << "\n" << std::string(70, '-') << "\n";
    std::cout << "Final Balance:        INR " << final_balance << "\n";
    std::cout << "Total Return:         " << return_pct << "%\n";
    std::cout << std::string(70, '-') << "\n";
    
    Statistics stats = logger.calculateStatistics(
        config.getInitialBalance(), final_balance, engine.getEquityCurve());
    logger.printSummary(stats);
}

int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments
        Mode mode = Mode::BACKTEST;
        std::string config_file = "config.txt";
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--live") {
                mode = Mode::LIVE_SIM;
            } else if (arg == "--backtest") {
                mode = Mode::BACKTEST;
            } else if (arg == "--config" && i + 1 < argc) {
                config_file = argv[++i];
            }
        }
        
        // Load configuration
        Config config;
        config.loadFromFile(config_file);
        
        // Command line overrides config file
        if (argc > 1) {
            if (mode == Mode::LIVE_SIM) {
                runLiveSimulation(config);
            } else {
                runBacktest(config);
            }
        } else {
            // Use mode from config file
            if (config.getMode() == Mode::LIVE_SIM) {
                runLiveSimulation(config);
            } else {
                runBacktest(config);
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "\nFatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "\nUnknown fatal error occurred" << std::endl;
        return 1;
    }
    
    return 0;
}