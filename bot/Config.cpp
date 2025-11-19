#include "Config.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace TradingBot {

Config::Config() {
    setDefaults();
}

void Config::setDefaults() {
    initial_balance_ = 100000.0;     // INR 1,00,000
    risk_percentage_ = 2.0;           // 2% risk per trade
    max_exposure_ = 80.0;             // Max 80% capital in positions
    stop_loss_percentage_ = 5.0;      // 5% stop loss
    take_profit_percentage_ = 10.0;   // 10% take profit
    data_file_path_ = "data/nifty50_data.csv";
    symbol_ = "NIFTY50";
    short_ma_period_ = 20;            // 20-day moving average
    long_ma_period_ = 50;             // 50-day moving average
    rsi_period_ = 14;                 // 14-day RSI
    mode_ = Mode::BACKTEST;
    
    // Defaults for live simulation
    live_iterations_ = 100;
    live_history_size_ = 200;
    live_delay_ms_ = 500;
}

bool Config::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file '" << filename 
                  << "'. Using defaults.\n";
        return false;
    }
    
    std::string line;
    int line_num = 0;
    
    while (std::getline(file, line)) {
        line_num++;
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;
        
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            std::cerr << "Warning: Invalid format at line " << line_num 
                      << ": " << line << std::endl;
            continue;
        }
        
        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        
        // Remove whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        try {
            if (key == "initial_balance") {
                initial_balance_ = std::stod(value);
            } else if (key == "risk_percentage") {
                risk_percentage_ = std::stod(value);
            } else if (key == "max_exposure") {
                max_exposure_ = std::stod(value);
            } else if (key == "stop_loss_percentage") {
                stop_loss_percentage_ = std::stod(value);
            } else if (key == "take_profit_percentage") {
                take_profit_percentage_ = std::stod(value);
            } else if (key == "data_file_path") {
                data_file_path_ = value;
            } else if (key == "symbol") {
                symbol_ = value;
            } else if (key == "short_ma_period") {
                short_ma_period_ = std::stoi(value);
            } else if (key == "long_ma_period") {
                long_ma_period_ = std::stoi(value);
            } else if (key == "rsi_period") {
                rsi_period_ = std::stoi(value);
            } else if (key == "mode") {
                mode_ = (value == "live") ? Mode::LIVE_SIM : Mode::BACKTEST;
            } else if (key == "live_iterations") {
                live_iterations_ = std::stoi(value);
            } else if (key == "live_history_size") {
                live_history_size_ = std::stoi(value);
            } else if (key == "live_delay_ms") {
                live_delay_ms_ = std::stoi(value);
            }
        } catch (const std::exception& e) {
            std::cerr << "Error parsing value for '" << key << "' at line " 
                      << line_num << ": " << e.what() << std::endl;
        }
    }
    
    file.close();
    return validate();
}

bool Config::validateNumeric(const std::string& name, double value, 
                             double min, double max) {
    if (value < min || value > max) {
        std::cerr << "Error: " << name << " (" << value 
                  << ") must be between " << min << " and " << max << std::endl;
        return false;
    }
    return true;
}

bool Config::validate() {
    bool valid = true;
    
    valid &= validateNumeric("initial_balance", initial_balance_, 1000, 1e9);
    valid &= validateNumeric("risk_percentage", risk_percentage_, 0.1, 10.0);
    valid &= validateNumeric("max_exposure", max_exposure_, 10.0, 100.0);
    valid &= validateNumeric("stop_loss_percentage", stop_loss_percentage_, 1.0, 50.0);
    valid &= validateNumeric("take_profit_percentage", take_profit_percentage_, 1.0, 100.0);
    
    if (short_ma_period_ <= 0 || short_ma_period_ >= long_ma_period_) {
        std::cerr << "Error: short_ma_period (" << short_ma_period_ 
                  << ") must be positive and less than long_ma_period (" 
                  << long_ma_period_ << ")" << std::endl;
        valid = false;
    }
    
    if (long_ma_period_ <= 0 || long_ma_period_ > 200) {
        std::cerr << "Error: long_ma_period must be between 1 and 200" << std::endl;
        valid = false;
    }
    
    if (rsi_period_ < 2 || rsi_period_ > 50) {
        std::cerr << "Error: rsi_period must be between 2 and 50" << std::endl;
        valid = false;
    }
    
    if (!valid) {
        std::cerr << "\nConfiguration validation failed. Please check config.txt\n";
    }
    
    return valid;
}

void Config::printConfig() const {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "            CONFIGURATION\n";
    std::cout << std::string(60, '=') << "\n";
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Mode:                  " << (mode_ == Mode::BACKTEST ? "BACKTEST" : "LIVE SIMULATION") << "\n";
    std::cout << "Initial Balance:       INR " << initial_balance_ << "\n";
    std::cout << "Risk per Trade:        " << risk_percentage_ << "%\n";
    std::cout << "Max Exposure:          " << max_exposure_ << "%\n";
    std::cout << "Stop Loss:             " << stop_loss_percentage_ << "%\n";
    std::cout << "Take Profit:           " << take_profit_percentage_ << "%\n";
    std::cout << "Symbol:                " << symbol_ << "\n";
    std::cout << "Short MA Period:       " << short_ma_period_ << " days\n";
    std::cout << "Long MA Period:        " << long_ma_period_ << " days\n";
    std::cout << "RSI Period:            " << rsi_period_ << " days\n";
    std::cout << "Data File:             " << data_file_path_ << "\n";
    if (mode_ == Mode::LIVE_SIM) {
        std::cout << "Live Iterations:       " << live_iterations_ << "\n";
        std::cout << "Live History Size:     " << live_history_size_ << "\n";
        std::cout << "Live Delay:            " << live_delay_ms_ << " ms\n";
    }
    std::cout << std::string(60, '=') << "\n\n";
}

}