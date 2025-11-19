#ifndef STRATEGY_H
#define STRATEGY_H

#include <vector>
#include <deque>
#include "Types.h"

namespace TradingBot {

class Strategy {
public:
    Strategy(int short_period, int long_period, int rsi_period = 14);
    
    Signal generateSignal(const std::vector<OHLCV>& data, size_t current_index, 
                         std::string& reason);
    
    bool shouldExitPosition(double entry_price, double current_price,
                           double stop_loss_pct, double take_profit_pct,
                           std::string& reason);
    
    double calculateSMA(const std::vector<OHLCV>& data, size_t end_index, int period);
    double calculateRSI(const std::vector<OHLCV>& data, size_t end_index, int period = 14);
    
    // Get current indicator values for logging
    double getLastShortMA() const { return last_short_ma_; }
    double getLastLongMA() const { return last_long_ma_; }
    double getLastRSI() const { return last_rsi_; }

private:
    int short_period_;
    int long_period_;
    int rsi_period_;
    
    Signal previous_signal_;
    
    // Cache last calculated values
    double last_short_ma_;
    double last_long_ma_;
    double last_rsi_;
    
    // For detecting crossovers
    double prev_short_ma_;
    double prev_long_ma_;
    
    bool isGoldenCross() const;
    bool isDeathCross() const;
};

}

#endif