#include "MarketDataHandler.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <cmath>

namespace TradingBot {

MarketDataHandler::MarketDataHandler() {
    // Initialize random number generator with better seed
    auto seed = static_cast<unsigned int>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
    rng_.seed(seed);
}

MarketDataHandler::~MarketDataHandler() {}

std::string MarketDataHandler::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

bool MarketDataHandler::validateOHLCV(const OHLCV& data, int line_number) {
    if (!data.isValid()) {
        std::cerr << "Warning: Invalid OHLCV data at line " << line_number 
                  << " (O:" << data.open << " H:" << data.high 
                  << " L:" << data.low << " C:" << data.close << ")" << std::endl;
        return false;
    }
    return true;
}

OHLCV MarketDataHandler::parseCSVLine(const std::string& line, int line_number) {
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
            std::cerr << "Error parsing field " << field << " at line " 
                      << line_number << ": " << token << " (" << e.what() << ")" << std::endl;
            return OHLCV();  // Return invalid data
        }
        field++;
    }
    
    if (field < 6) {
        std::cerr << "Error: Incomplete CSV line at " << line_number 
                  << " (expected 6 fields, got " << field << ")" << std::endl;
        return OHLCV();
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
    int line_number = 0;
    int valid_count = 0;
    int invalid_count = 0;
    
    while (std::getline(file, line)) {
        line_number++;
        
        if (first_line) {
            first_line = false;
            continue;  // Skip header
        }
        
        if (line.empty()) continue;
        
        OHLCV data = parseCSVLine(line, line_number);
        
        if (validateOHLCV(data, line_number)) {
            data_.push_back(data);
            valid_count++;
        } else {
            invalid_count++;
        }
    }
    
    file.close();
    
    std::cout << "Loaded " << valid_count << " valid data points from " << filename;
    if (invalid_count > 0) {
        std::cout << " (" << invalid_count << " invalid lines skipped)";
    }
    std::cout << std::endl;
    
    if (data_.empty()) {
        std::cerr << "Error: No valid data loaded from file" << std::endl;
        return false;
    }
    
    return true;
}

const OHLCV& MarketDataHandler::getDataAt(size_t index) const {
    if (index >= data_.size()) {
        throw std::out_of_range("Index out of range in market data");
    }
    return data_[index];
}

OHLCV MarketDataHandler::generateLiveData(const OHLCV& previous) {
    // Improved live data generation with more realistic behavior
    
    // Use normal distribution for price changes (mean-reverting with trend)
    std::normal_distribution<> price_change_dist(0.0, 0.008);  // ~0.8% daily volatility
    std::uniform_real_distribution<> intraday_range(0.005, 0.02);  // 0.5-2% intraday range
    std::normal_distribution<> volume_change_dist(0.0, 0.15);  // 15% volume volatility
    
    OHLCV live_data;
    
    // Generate price change
    double change_percent = price_change_dist(rng_);
    
    // Opening price (gap from previous close)
    std::normal_distribution<> gap_dist(0.0, 0.003);  // ~0.3% gap
    double gap = gap_dist(rng_);
    live_data.open = previous.close * (1.0 + gap);
    
    // Close price (apply main trend)
    live_data.close = live_data.open * (1.0 + change_percent);
    
    // High and Low based on intraday volatility
    double range_pct = intraday_range(rng_);
    double mid_price = (live_data.open + live_data.close) / 2.0;
    
    live_data.high = mid_price * (1.0 + range_pct / 2.0);
    live_data.low = mid_price * (1.0 - range_pct / 2.0);
    
    // Ensure high/low constraints
    live_data.high = std::max({live_data.high, live_data.open, live_data.close});
    live_data.low = std::min({live_data.low, live_data.open, live_data.close});
    
    // Volume (correlated with price movement)
    double vol_change = volume_change_dist(rng_);
    double vol_mult = 1.0 + vol_change + std::abs(change_percent) * 2.0;  // Higher volume on big moves
    live_data.volume = static_cast<long long>(previous.volume * vol_mult);
    live_data.volume = std::max<long long>(1000000, live_data.volume);  // Min 1M volume
    
    return live_data;
}

}