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
    bool validate();  // Validate configuration values
    
    double getInitialBalance() const { return initial_balance_; }
    double getRiskPercentage() const { return risk_percentage_; }
    double getMaxExposure() const { return max_exposure_; }
    double getStopLossPercentage() const { return stop_loss_percentage_; }
    double getTakeProfitPercentage() const { return take_profit_percentage_; }
    std::string getDataFilePath() const { return data_file_path_; }
    std::string getSymbol() const { return symbol_; }
    int getShortMA() const { return short_ma_period_; }
    int getLongMA() const { return long_ma_period_; }
    int getRSIPeriod() const { return rsi_period_; }
    Mode getMode() const { return mode_; }
    
    // Live simulation parameters
    int getLiveIterations() const { return live_iterations_; }
    int getLiveHistorySize() const { return live_history_size_; }
    int getLiveDelayMs() const { return live_delay_ms_; }
    
    void printConfig() const;

private:
    double initial_balance_;
    double risk_percentage_;
    double max_exposure_;
    double stop_loss_percentage_;
    double take_profit_percentage_;
    std::string data_file_path_;
    std::string symbol_;
    int short_ma_period_;
    int long_ma_period_;
    int rsi_period_;
    Mode mode_;
    
    int live_iterations_;
    int live_history_size_;
    int live_delay_ms_;
    
    void setDefaults();
    bool validateNumeric(const std::string& name, double value, double min, double max);
};

}

#endif