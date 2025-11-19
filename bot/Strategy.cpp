#include "Strategy.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace TradingBot {

Strategy::Strategy(int short_period, int long_period, int rsi_period)
    : short_period_(short_period), long_period_(long_period), 
      rsi_period_(rsi_period), previous_signal_(Signal::HOLD),
      last_short_ma_(0), last_long_ma_(0), last_rsi_(50),
      prev_short_ma_(0), prev_long_ma_(0) {}

double Strategy::calculateSMA(const std::vector<OHLCV>& data, 
                              size_t end_index, int period) {
    if (end_index + 1 < static_cast<size_t>(period) || data.empty()) {
        return 0.0;
    }
    
    double sum = 0.0;
    for (int i = 0; i < period; ++i) {
        sum += data[end_index - i].close;
    }
    
    return sum / period;
}

double Strategy::calculateRSI(const std::vector<OHLCV>& data, 
                              size_t end_index, int period) {
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

bool Strategy::isGoldenCross() const {
    // Golden cross: short MA crosses above long MA
    return (prev_short_ma_ < prev_long_ma_ || 
            std::abs(prev_short_ma_ - prev_long_ma_) < 0.01) && 
           last_short_ma_ > last_long_ma_;
}

bool Strategy::isDeathCross() const {
    // Death cross: short MA crosses below long MA
    return (prev_short_ma_ > prev_long_ma_ || 
            std::abs(prev_short_ma_ - prev_long_ma_) < 0.01) && 
           last_short_ma_ < last_long_ma_;
}

Signal Strategy::generateSignal(const std::vector<OHLCV>& data, 
                                size_t current_index, std::string& reason) {
    if (current_index < static_cast<size_t>(long_period_)) {
        reason = "Insufficient data for indicators";
        return Signal::HOLD;
    }
    
    // Store previous values
    prev_short_ma_ = last_short_ma_;
    prev_long_ma_ = last_long_ma_;
    
    // Calculate current indicators
    last_short_ma_ = calculateSMA(data, current_index, short_period_);
    last_long_ma_ = calculateSMA(data, current_index, long_period_);
    last_rsi_ = calculateRSI(data, current_index, rsi_period_);
    
    Signal signal = Signal::HOLD;
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    
    // Check for golden cross (BUY signal)
    if (isGoldenCross() && last_rsi_ < 70) {
        signal = Signal::BUY;
        oss << "Golden Cross detected (MA" << short_period_ << "=" << last_short_ma_
            << " > MA" << long_period_ << "=" << last_long_ma_ 
            << ", RSI=" << last_rsi_ << ")";
    }
    // Check for death cross (SELL signal)
    else if (isDeathCross() && last_rsi_ > 30) {
        signal = Signal::SELL;
        oss << "Death Cross detected (MA" << short_period_ << "=" << last_short_ma_
            << " < MA" << long_period_ << "=" << last_long_ma_ 
            << ", RSI=" << last_rsi_ << ")";
    }
    else {
        // Provide detailed reason for HOLD
        if (last_short_ma_ > last_long_ma_) {
            oss << "Bullish trend but no crossover (MA" << short_period_ 
                << "=" << last_short_ma_ << " > MA" << long_period_ 
                << "=" << last_long_ma_;
            if (last_rsi_ >= 70) {
                oss << ", RSI overbought=" << last_rsi_;
            }
            oss << ")";
        } else {
            oss << "Bearish trend but no crossover (MA" << short_period_ 
                << "=" << last_short_ma_ << " < MA" << long_period_ 
                << "=" << last_long_ma_;
            if (last_rsi_ <= 30) {
                oss << ", RSI oversold=" << last_rsi_;
            }
            oss << ")";
        }
    }
    
    reason = oss.str();
    previous_signal_ = signal;
    return signal;
}

bool Strategy::shouldExitPosition(double entry_price, double current_price,
                                  double stop_loss_pct, double take_profit_pct,
                                  std::string& reason) {
    double price_change_pct = ((current_price - entry_price) / entry_price) * 100.0;
    
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2);
    
    if (price_change_pct <= -stop_loss_pct) {
        oss << "Stop loss triggered (" << price_change_pct << "%)";
        reason = oss.str();
        return true;
    }
    
    if (price_change_pct >= take_profit_pct) {
        oss << "Take profit triggered (" << price_change_pct << "%)";
        reason = oss.str();
        return true;
    }
    
    return false;
}

}