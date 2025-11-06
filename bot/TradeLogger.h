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
    void logTrade(const Trade& trade); //log trade
    Statistics calculateStatistics(double initial_balance, double final_balance); // return trade stats
    void printSummary(const Statistics& stats); // print summary   
private:
    std::ofstream log_file_;
    std::vector<Trade> trades_;
    void writeHeader();
};
} 

#endif 
