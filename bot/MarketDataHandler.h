#ifndef MARKET_DATA_HANDLER_H
#define MARKET_DATA_HANDLER_H

#include <string>
#include <vector>
#include <fstream>
#include "Types.h"

namespace TradingBot {
class MarketDataHandler {
public:
    MarketDataHandler();
    ~MarketDataHandler();
    bool loadFromCSV(const std::string& filename); //load csv
    const OHLCV& getDataAt(size_t index) const;
    size_t getDataSize() const { return data_.size(); }
    const std::vector<OHLCV>& getAllData() const { return data_; }    
    OHLCV generateLiveData(const OHLCV& previous);
    
private:
    std::vector<OHLCV> data_;    
    OHLCV parseCSVLine(const std::string& line);  // Helper function to parse CSV line
    std::string trim(const std::string& str);
};
}

#endif
