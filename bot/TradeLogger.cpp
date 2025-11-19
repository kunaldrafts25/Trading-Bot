#include "TradeLogger.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <numeric>

namespace TradingBot {

TradeLogger::TradeLogger(const std::string& log_file) {
    log_file_.open(log_file);
    if (log_file_.is_open()) {
        writeHeader();
        std::cout << "Trade log created: " << log_file << std::endl;
    } else {
        std::cerr << "Warning: Could not create trade log file: " << log_file << std::endl;
    }
}

TradeLogger::~TradeLogger() {
    if (log_file_.is_open()) {
        log_file_.close();
    }
}

void TradeLogger::writeHeader() {
    log_file_ << "Timestamp,Symbol,Signal,Price,Quantity,Value,ProfitLoss,Balance,Reason\n";
}

void TradeLogger::logTrade(const Trade& trade) {
    trades_.push_back(trade);
    
    if (log_file_.is_open()) {
        log_file_ << trade.timestamp << ","
                  << trade.symbol << ","
                  << (trade.signal == Signal::BUY ? "BUY" : "SELL") << ","
                  << std::fixed << std::setprecision(2) << trade.price << ","
                  << trade.quantity << ","
                  << trade.value << ","
                  << trade.profit_loss << ","
                  << trade.balance_after << ","
                  << "\"" << trade.reason << "\"\n";
        log_file_.flush();
    }
}

double TradeLogger::calculateMaxDrawdown(const std::vector<double>& equity_curve) {
    if (equity_curve.empty()) return 0.0;
    
    double max_dd = 0.0;
    double peak = equity_curve[0];
    
    for (double equity : equity_curve) {
        if (equity > peak) {
            peak = equity;
        }
        double dd = ((peak - equity) / peak) * 100.0;
        max_dd = std::max(max_dd, dd);
    }
    
    return max_dd;
}

double TradeLogger::calculateSharpeRatio(const std::vector<double>& returns) {
    if (returns.size() < 2) return 0.0;
    
    double mean = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    
    double variance = 0.0;
    for (double ret : returns) {
        variance += (ret - mean) * (ret - mean);
    }
    variance /= returns.size();
    
    double std_dev = std::sqrt(variance);
    
    if (std_dev == 0) return 0.0;
    
    // Assuming risk-free rate of 0 for simplicity
    // Annualize (assuming daily returns, 252 trading days)
    return (mean / std_dev) * std::sqrt(252.0);
}

Statistics TradeLogger::calculateStatistics(double initial_balance, 
                                           double final_balance,
                                           const std::vector<double>& equity_curve) {
    Statistics stats;
    
    stats.total_trades = static_cast<int>(trades_.size());
    stats.total_profit_loss = final_balance - initial_balance;
    
    // Analyze profits and losses
    std::vector<double> profits;
    std::vector<double> losses;
    double total_profit = 0.0;
    double total_loss = 0.0;
    
    for (const Trade& trade : trades_) {
        if (trade.signal == Signal::SELL && trade.profit_loss != 0) {
            if (trade.profit_loss > 0) {
                stats.winning_trades++;
                profits.push_back(trade.profit_loss);
                total_profit += trade.profit_loss;
                stats.largest_win = std::max(stats.largest_win, trade.profit_loss);
            } else {
                stats.losing_trades++;
                losses.push_back(trade.profit_loss);
                total_loss += std::abs(trade.profit_loss);
                stats.largest_loss = std::min(stats.largest_loss, trade.profit_loss);
            }
        }
    }
    
    // Calculate win rate
    int closed_trades = stats.winning_trades + stats.losing_trades;
    if (closed_trades > 0) {
        stats.win_rate = (static_cast<double>(stats.winning_trades) / closed_trades) * 100.0;
    }
    
    // Calculate average win/loss
    if (!profits.empty()) {
        stats.avg_win = total_profit / profits.size();
    }
    if (!losses.empty()) {
        stats.avg_loss = total_loss / losses.size();
    }
    
    // Calculate profit factor
    if (total_loss > 0) {
        stats.profit_factor = total_profit / total_loss;
    }
    
    // Calculate max drawdown
    stats.max_drawdown = calculateMaxDrawdown(equity_curve);
    
    // Calculate Sharpe ratio
    if (equity_curve.size() > 1) {
        std::vector<double> returns;
        for (size_t i = 1; i < equity_curve.size(); ++i) {
            double ret = (equity_curve[i] - equity_curve[i-1]) / equity_curve[i-1];
            returns.push_back(ret);
        }
        stats.sharpe_ratio = calculateSharpeRatio(returns);
    }
    
    return stats;
}

void TradeLogger::printSummary(const Statistics& stats) {
    std::cout << "\n" << std::string(70, '=') << "\n";
    std::cout << "                     TRADING SUMMARY\n";
    std::cout << std::string(70, '=') << "\n";
    std::cout << std::fixed << std::setprecision(2);
    
    std::cout << std::left << std::setw(30) << "Total Trades:" 
              << stats.total_trades << "\n";
    std::cout << std::left << std::setw(30) << "Winning Trades:" 
              << stats.winning_trades << "\n";
    std::cout << std::left << std::setw(30) << "Losing Trades:" 
              << stats.losing_trades << "\n";
    std::cout << std::left << std::setw(30) << "Win Rate:" 
              << stats.win_rate << "%\n";
    
    std::cout << "\n";
    std::cout << std::left << std::setw(30) << "Total P/L:" 
              << "INR " << stats.total_profit_loss << "\n";
    std::cout << std::left << std::setw(30) << "Average Win:" 
              << "INR " << stats.avg_win << "\n";
    std::cout << std::left << std::setw(30) << "Average Loss:" 
              << "INR " << stats.avg_loss << "\n";
    std::cout << std::left << std::setw(30) << "Largest Win:" 
              << "INR " << stats.largest_win << "\n";
    std::cout << std::left << std::setw(30) << "Largest Loss:" 
              << "INR " << stats.largest_loss << "\n";
    
    std::cout << "\n";
    std::cout << std::left << std::setw(30) << "Profit Factor:" 
              << stats.profit_factor << "\n";
    std::cout << std::left << std::setw(30) << "Max Drawdown:" 
              << stats.max_drawdown << "%\n";
    std::cout << std::left << std::setw(30) << "Sharpe Ratio:" 
              << stats.sharpe_ratio << "\n";
    
    std::cout << std::string(70, '=') << "\n";
}

}