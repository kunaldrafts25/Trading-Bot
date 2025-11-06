#include "Strategy.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace TradingBot {
Strategy::Strategy(int short_period, int long_period)
    : short_period_(short_period), long_period_(long_period), previous_signal_(Signal::HOLD) {}
double Strategy::calculateSMA(const std::vector<OHLCV>& data, size_t end_index, int period) {

    // Simple Moving Average calculation
    if (end_index + 1 < static_cast<size_t>(period) || data.empty()) {
        return 0.0;
    }    
    double sum = 0.0;
    for (int i = 0; i < period; ++i) {
        sum += data[end_index - i].close;
    }    
    return sum / period;
}

double Strategy::calculateRSI(const std::vector<OHLCV>& data, size_t end_index, int period) {

    // RSI (Relative Strength Index) calculation
    if (end_index < static_cast<size_t>(period) || data.empty()) {
        return 50.0;  // Neutral RSI
    }    
    double gains = 0.0;
    double losses = 0.0;    
    for (int i = 1; i <= period; ++i) {
        double change = data[end_index - i + 1].close - data[end_index - i].close;
        if (change > 0) {
            gains += change;
        } else {
            losses += std::abs(change);
        }
    }    
    double avg_gain = gains / period;
    double avg_loss = losses / period;    
    if (avg_loss == 0) return 100.0;    
    double rs = avg_gain / avg_loss;
    double rsi = 100.0 - (100.0 / (1.0 + rs));
    
    return rsi;
}

Signal Strategy::generateSignal(const std::vector<OHLCV>& data, size_t current_index) {
    if (current_index < static_cast<size_t>(long_period_)) {
        return Signal::HOLD;
    }
    
    // Calculate moving averages
    double short_ma = calculateSMA(data, current_index, short_period_);
    double long_ma = calculateSMA(data, current_index, long_period_);
    
    // Calculate previous moving averages to detect crossover
    double prev_short_ma = calculateSMA(data, current_index - 1, short_period_);
    double prev_long_ma = calculateSMA(data, current_index - 1, long_period_);
    
    double rsi = calculateRSI(data, current_index, 14);  
    Signal signal = Signal::HOLD;
    if (prev_short_ma <= prev_long_ma && short_ma > long_ma && rsi < 70) {  
        signal = Signal::BUY;  //Golden cross
    }
    else if (prev_short_ma >= prev_long_ma && short_ma < long_ma && rsi > 30) {  
        signal = Signal::SELL;  //death cross
    }
    if (signal != Signal::HOLD) {
        std::cout << "Signal detected: Short MA=" << short_ma 
                  << ", Long MA=" << long_ma << ", RSI=" << rsi << std::endl;
    }    
    previous_signal_ = signal;
    return signal;
}
} 
