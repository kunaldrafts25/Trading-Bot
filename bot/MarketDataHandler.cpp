#include "MarketDataHandler.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <random>
#include <chrono>

namespace TradingBot {

MarketDataHandler::MarketDataHandler() {}
MarketDataHandler::~MarketDataHandler() {}
std::string MarketDataHandler::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}
OHLCV MarketDataHandler::parseCSVLine(const std::string& line) {
    OHLCV data;
    std::stringstream ss(line);
    std::string token;
    int field = 0;
    
    while (std::getline(ss, token, ',')) {
        token = trim(token);        
        try {
            switch (field) {
                case 0: data.date = token; break;
                case 1: data.open = std::stod(token); break;
                case 2: data.high = std::stod(token); break;
                case 3: data.low = std::stod(token); break;
                case 4: data.close = std::stod(token); break;
                case 5: data.volume = std::stoll(token); break;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error parsing field " << field << ": " << token << std::endl;
        }
        field++;
    }    
    return data;
}
bool MarketDataHandler::loadFromCSV(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open data file: " << filename << std::endl;
        return false;
    }    
    std::string line;
    bool first_line = true;    
    while (std::getline(file, line)) {
        if (first_line) {
            first_line = false;
            continue;
        }        
        if (line.empty()) continue;        
        OHLCV data = parseCSVLine(line);
        if (data.close > 0) { 
            data_.push_back(data);
        }
    }    
    file.close();    
    std::cout << "Loaded " << data_.size() << " data points from " << filename << std::endl;
    return !data_.empty();
}
const OHLCV& MarketDataHandler::getDataAt(size_t index) const {
    if (index >= data_.size()) {
        throw std::out_of_range("Index out of range in market data");
    }
    return data_[index];
}
OHLCV MarketDataHandler::generateLiveData(const OHLCV& previous) {
    
    // Simulate random price movement for live mode
    static std::random_device rd;
    static std::mt19937 gen(static_cast<unsigned int>(
        rd() ^ static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count())));

    // Uses a normal distribution for percent change (in basis points)
    static std::normal_distribution<> price_change(0.0, 0.5);  // Mean 0, StdDev 0.5%
    std::normal_distribution<> vol_change_dist(0.0, 200000.0);

    OHLCV live_data;
    double change_percent = price_change(gen) / 100.0;    
    live_data.open = previous.close;
    live_data.close = previous.close * (1.0 + change_percent);
    live_data.high = std::max(live_data.open, live_data.close) * 1.002;
    live_data.low = std::min(live_data.open, live_data.close) * 0.998;
    long long vdelta = static_cast<long long>(std::llround(vol_change_dist(gen)));
    live_data.volume = std::max<long long>(0, static_cast<long long>(previous.volume) + vdelta);
    return live_data;
}
} 
