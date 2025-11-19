#ifndef TRADE_LOGGER_H
#define TRADE_LOGGER_H

#include <string>
#include <vector>
#include <fstream>
#include "Types.h"

namespace TradingBot {

class TradeLogger {
public:
    TradeLogger(const std::string& log_file);
    ~TradeLogger();
    
    void logTrade(const Trade& trade);
    
    Statistics calculateStatistics(double initial_balance, double final_balance,
                                   const std::vector<double>& equity_curve);
    
    void printSummary(const Statistics& stats);
    
    const std::vector<Trade>& getTrades() const { return trades_; }
    
private:
    std::ofstream log_file_;
    std::vector<Trade> trades_;
    
    void writeHeader();
    double calculateMaxDrawdown(const std::vector<double>& equity_curve);
    double calculateSharpeRatio(const std::vector<double>& returns);
};

}

#endif