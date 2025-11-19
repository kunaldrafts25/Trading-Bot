#ifndef MARKET_DATA_HANDLER_H
#define MARKET_DATA_HANDLER_H

#include <string>
#include <vector>
#include <fstream>
#include <random>
#include "Types.h"

namespace TradingBot {

class MarketDataHandler {
public:
    MarketDataHandler();
    ~MarketDataHandler();
    
    bool loadFromCSV(const std::string& filename);
    const OHLCV& getDataAt(size_t index) const;
    size_t getDataSize() const { return data_.size(); }
    const std::vector<OHLCV>& getAllData() const { return data_; }
    
    // Live simulation data generation (improved)
    OHLCV generateLiveData(const OHLCV& previous);
    
private:
    std::vector<OHLCV> data_;
    
    // Random number generator for live simulation
    std::mt19937 rng_;
    
    OHLCV parseCSVLine(const std::string& line, int line_number);
    std::string trim(const std::string& str);
    bool validateOHLCV(const OHLCV& data, int line_number);
};

}

#endif