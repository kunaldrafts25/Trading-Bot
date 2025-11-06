#include "TradeLogger.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace TradingBot {
TradeLogger::TradeLogger(const std::string& log_file) {
    log_file_.open(log_file);
    if (log_file_.is_open()) {
        writeHeader();
        std::cout << "Trade log created: " << log_file << std::endl;
    } else {
        std::cerr << "Warning: Could not create trade log file" << std::endl;
    }
}
TradeLogger::~TradeLogger() {
    if (log_file_.is_open()) {
        log_file_.close();
    }
}
void TradeLogger::writeHeader() {
    log_file_ << "Timestamp,Symbol,Signal,Price,Quantity,Value,Balance\n";
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
                  << trade.balance_after << "\n";
        log_file_.flush();  // Ensure data is written immediately
    }
}
Statistics TradeLogger::calculateStatistics(double initial_balance, double final_balance) {
    Statistics stats;    
    stats.total_trades = static_cast<int>(trades_.size());
    stats.total_profit_loss = final_balance - initial_balance;
    
    // Analyze each trade pair (buy-sell)
    std::vector<double> profits;    
    for (size_t i = 1; i < trades_.size(); i += 2) {
        if (i >= trades_.size()) break;        
        if (trades_[i-1].signal == Signal::BUY && trades_[i].signal == Signal::SELL) {
            double profit = trades_[i].value - trades_[i-1].value;
            profits.push_back(profit);
            
            if (profit > 0) {
                stats.winning_trades++;
                stats.largest_win = std::max(stats.largest_win, profit);
            } else {
                stats.losing_trades++;
                stats.largest_loss = std::min(stats.largest_loss, profit);
            }
        }
    }    
    if (stats.total_trades > 0) {
        stats.win_rate = (static_cast<double>(stats.winning_trades) / 
                         (stats.winning_trades + stats.losing_trades)) * 100.0;
    }    
    return stats;
}
void TradeLogger::printSummary(const Statistics& stats) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "            TRADING SUMMARY\n";
    std::cout << std::string(60, '=') << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Total Trades:        " << stats.total_trades << "\n";
    std::cout << "Winning Trades:      " << stats.winning_trades << "\n";
    std::cout << "Losing Trades:       " << stats.losing_trades << "\n";
    std::cout << "Win Rate:            " << stats.win_rate << "%\n";
    std::cout << "Total P/L:           INR " << stats.total_profit_loss << "\n";
    std::cout << "Largest Win:         INR " << stats.largest_win << "\n";
    std::cout << "Largest Loss:        INR " << stats.largest_loss << "\n";
    std::cout << std::string(60, '=') << "\n";
}
} 
