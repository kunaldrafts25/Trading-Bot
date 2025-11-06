#ifndef STRATEGY_H
#define STRATEGY_H

#include <vector>
#include "Types.h"

namespace TradingBot {

class Strategy {
public:
    Strategy(int short_period, int long_period);
    Signal generateSignal(const std::vector<OHLCV>& data, size_t current_index); // Generate trading signal
    double calculateSMA(const std::vector<OHLCV>& data, size_t end_index, int period); //calculate SMA
    double calculateRSI(const std::vector<OHLCV>& data, size_t end_index, int period = 14); // calculate RSI   
private:
    int short_period_;  // Short-term MA period
    int long_period_;   // Long-term MA period
    
    Signal previous_signal_;  // Track previous signal to avoid rapid switching
};
} 

#endif 
