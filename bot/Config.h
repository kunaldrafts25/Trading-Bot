#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include "Types.h"
namespace TradingBot {
class Config {
public:
    Config();
    bool loadFromFile(const std::string& filename);    
    double getInitialBalance() const { return initial_balance_; }
    double getRiskPercentage() const { return risk_percentage_; }
    std::string getDataFilePath() const { return data_file_path_; }
    std::string getSymbol() const { return symbol_; }
    int getShortMA() const { return short_ma_period_; }
    int getLongMA() const { return long_ma_period_; }
    Mode getMode() const { return mode_; }    
    // Live simulation parameters
    int getLiveIterations() const { return live_iterations_; }
    int getLiveHistorySize() const { return live_history_size_; }
private:
    double initial_balance_;
    double risk_percentage_;
    std::string data_file_path_;
    std::string symbol_;
    int short_ma_period_;
    int long_ma_period_;
    Mode mode_;    
    int live_iterations_;
    int live_history_size_;
    void setDefaults();
};
}

#endif 
