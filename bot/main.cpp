#include <iostream>
#include <thread>
#include <chrono>
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

// Function to run backtest mode
void runBacktest(Config& config) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "    NIFTY 50 TRADING BOT - BACKTEST\n";
    std::cout << std::string(60, '=') << "\n\n";
    
    // Initialize components
    MarketDataHandler dataHandler;
    if (!dataHandler.loadFromCSV(config.getDataFilePath())) {
        std::cerr << "Failed to load market data. Exiting.\n";
        return;
    }   
    Strategy strategy(config.getShortMA(), config.getLongMA());
    RiskManager riskManager(config.getRiskPercentage());
    BrokerSimulator broker(config.getInitialBalance());
    TradeLogger logger("logs/trades.csv");
    
    std::cout << "Initial Balance: INR " << config.getInitialBalance() << "\n";
    std::cout << "Strategy: MA Crossover (" << config.getShortMA() 
              << "/" << config.getLongMA() << ")\n";
    std::cout << "Risk per Trade: " << config.getRiskPercentage() << "%\n\n";
    
    // Run backtest
    const auto& all_data = dataHandler.getAllData();
    size_t total_bars = all_data.size();
    
    for (size_t i = config.getLongMA(); i < total_bars; ++i) {
        const OHLCV& current_bar = all_data[i];
        
        // Generate signal
        Signal signal = strategy.generateSignal(all_data, i);
        
        // Execute trades based on signal
        if (signal == Signal::BUY && !broker.hasPosition(config.getSymbol())) {
            int quantity = riskManager.calculatePositionSize(
                current_bar.close, broker.getBalance());
            
            if (quantity > 0) {
                if (broker.executeBuy(config.getSymbol(), quantity, 
                                     current_bar.close, current_bar.date)) {
                    logger.logTrade(broker.getLastTrade());
                }
            }
        } 
        else if (signal == Signal::SELL && broker.hasPosition(config.getSymbol())) {
            Position pos = broker.getPosition(config.getSymbol());
            if (broker.executeSell(config.getSymbol(), pos.quantity, 
                                  current_bar.close, current_bar.date)) {
                logger.logTrade(broker.getLastTrade());
            }
        }
        
        // Show progress every 10%
        if (i % (total_bars / 10) == 0) {
            double progress = (static_cast<double>(i) / total_bars) * 100.0;
            double portfolio_value = broker.getPortfolioValue(
                config.getSymbol(), current_bar.close);
            std::cout << "Progress: " << static_cast<int>(progress) << "% | "
                      << "Date: " << current_bar.date << " | "
                      << "Price: INR " << current_bar.close << " | "
                      << "Portfolio Value: INR " << portfolio_value << "\n";
        }
    }
    
    // Close any remaining positions
    if (broker.hasPosition(config.getSymbol())) {
        const OHLCV& last_bar = all_data.back();
        Position pos = broker.getPosition(config.getSymbol());
        broker.executeSell(config.getSymbol(), pos.quantity, 
                          last_bar.close, last_bar.date);
        logger.logTrade(broker.getLastTrade());
    }
    
    // Calculate and display final statistics
    double final_balance = broker.getBalance();
    Statistics stats = logger.calculateStatistics(config.getInitialBalance(), final_balance);
    
    std::cout << "\nFinal Balance: INR " << final_balance << "\n";
    logger.printSummary(stats);
}

// Function to run live simulation mode
void runLiveSimulation(Config& config) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "    NIFTY 50 TRADING BOT - LIVE SIMULATION MODE\n";
    std::cout << std::string(60, '=') << "\n\n";    
    MarketDataHandler dataHandler;
    if (!dataHandler.loadFromCSV(config.getDataFilePath())) {
        std::cerr << "Failed to load market data. Exiting.\n";
        return;
    }   
    Strategy strategy(config.getShortMA(), config.getLongMA());
    RiskManager riskManager(config.getRiskPercentage());
    BrokerSimulator broker(config.getInitialBalance());
    TradeLogger logger("logs/live_trades.csv");
    
    std::cout << "Starting live simulation...\n";
    std::cout << "Press Ctrl+C to stop.\n\n";
    
    // Use data points as history, then simulate live
    std::vector<OHLCV> live_data;
    const auto& historical_data = dataHandler.getAllData();
    
    size_t history_size = static_cast<size_t>(config.getLiveHistorySize());
    size_t start_index = historical_data.size() > history_size ? 
                         historical_data.size() - history_size : 0;
    for (size_t i = start_index; i < historical_data.size(); ++i) {
        live_data.push_back(historical_data[i]);
    }
    
    // Simulate live trading for configured number of iterations
    int iterations = config.getLiveIterations();
    for (int iteration = 0; iteration < iterations; ++iteration) {
        // Generate new "live" data point
        OHLCV new_data = dataHandler.generateLiveData(live_data.back());
        new_data.date = "Live-" + std::to_string(iteration);
        live_data.push_back(new_data);

        // Generate signal
        Signal signal = strategy.generateSignal(live_data, live_data.size() - 1);
        
        std::cout << "\n--- Iteration " << iteration << " ---\n";
    std::cout << "Current Price: INR " << new_data.close << " | Signal: ";
        
        if (signal == Signal::BUY) {
            std::cout << "BUY\n";
            if (!broker.hasPosition(config.getSymbol())) {
                int quantity = riskManager.calculatePositionSize(
                    new_data.close, broker.getBalance());
                if (quantity > 0) {
                    broker.executeBuy(config.getSymbol(), quantity, 
                                     new_data.close, new_data.date);
                    logger.logTrade(broker.getLastTrade());
                }
            }
        } else if (signal == Signal::SELL) {
            std::cout << "SELL\n";
            if (broker.hasPosition(config.getSymbol())) {
                Position pos = broker.getPosition(config.getSymbol());
                broker.executeSell(config.getSymbol(), pos.quantity, 
                                  new_data.close, new_data.date);
                logger.logTrade(broker.getLastTrade());
            }
        } else {
            std::cout << "HOLD\n";
        }
        
        double portfolio_value = broker.getPortfolioValue(
            config.getSymbol(), new_data.close);
        std::cout << "Portfolio Value: INR " << portfolio_value << "\n";
        
    // Simulate real-time delay
#ifdef _WIN32
    ::Sleep(500); // Sleep takes milliseconds on Windows
#else
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
#endif
    }
    
    // Final summary
    double final_balance = broker.getBalance();
    Statistics stats = logger.calculateStatistics(config.getInitialBalance(), final_balance);
    logger.printSummary(stats);
}

int main(int argc, char* argv[]) {
    Mode mode = Mode::BACKTEST;
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--live") {
            mode = Mode::LIVE_SIM;
        }
    }
    Config config;
    config.loadFromFile("config.txt");
    if (argc > 1) {
        if (mode == Mode::LIVE_SIM) {
            runLiveSimulation(config);
        } else {
            runBacktest(config);
        }
    } else {
        if (config.getMode() == Mode::LIVE_SIM) {
            runLiveSimulation(config);
        } else {
            runBacktest(config);
        }
    }    
    return 0;
}
