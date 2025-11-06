#include "Config.h"
#include <fstream>
#include <iostream>
#include <sstream>

namespace TradingBot {
Config::Config() {
    setDefaults();
}
void Config::setDefaults() {
    initial_balance_ = 100000.0;     // INR 1,00,000
    risk_percentage_ = 2.0;           // 2% risk per trade
    data_file_path_ = "data/nifty50_data.csv";
    symbol_ = "NIFTY50";
    short_ma_period_ = 10;            // 10-day moving average
    long_ma_period_ = 50;             // 50-day moving average
    mode_ = Mode::BACKTEST;
    // Defaults for live simulation
    live_iterations_ = 100;
    live_history_size_ = 200;
}
bool Config::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Warning: Could not open config file. Using defaults.\n";
        return false;
    }    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            // Remove whitespace
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            
            if (key == "initial_balance") {
                initial_balance_ = std::stod(value);
            } else if (key == "risk_percentage") {
                risk_percentage_ = std::stod(value);
            } else if (key == "data_file_path") {
                data_file_path_ = value;
            } else if (key == "symbol") {
                symbol_ = value;
            } else if (key == "short_ma_period") {
                short_ma_period_ = std::stoi(value);
            } else if (key == "long_ma_period") {
                long_ma_period_ = std::stoi(value);
            } else if (key == "mode") {
                mode_ = (value == "live") ? Mode::LIVE_SIM : Mode::BACKTEST;
            } else if (key == "live_iterations") {
                try { live_iterations_ = std::stoi(value); } catch(...) {}
            } else if (key == "live_history_size") {
                try { live_history_size_ = std::stoi(value); } catch(...) {}
            }
        }
    }    
    file.close();
    return true;
}
} 
