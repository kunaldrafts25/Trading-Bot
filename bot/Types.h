#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>
#include <chrono>

namespace TradingBot {

// Enum for trade signal types
enum class Signal {
    BUY,
    SELL,
    HOLD
};

// Enum for trading mode
enum class Mode {
    BACKTEST,
    LIVE_SIM
};

// Enum for log levels
enum class LogLevel {
    DEBUG,
    INFO,
    WARN,
    ERROR_LVL
};

// Structure to hold OHLCV (Open, High, Low, Close, Volume) data
struct OHLCV {
    std::string date;
    double open;
    double high;
    double low;
    double close;
    long long volume;
    
    OHLCV() : open(0), high(0), low(0), close(0), volume(0) {}
    
    bool isValid() const {
        return open > 0 && high > 0 && low > 0 && close > 0 && 
               high >= low && high >= open && high >= close &&
               low <= open && low <= close;
    }
};

// Structure to represent a trade
struct Trade {
    std::string timestamp;
    std::string symbol;
    Signal signal;
    double price;
    int quantity;
    double value;
    double balance_after;
    double profit_loss;  // P/L for this trade
    std::string reason;  // Why this trade was made
    
    Trade() : price(0), quantity(0), value(0), balance_after(0), profit_loss(0) {}
};

// Structure to hold trading statistics
struct Statistics {
    int total_trades;
    int winning_trades;
    int losing_trades;
    double total_profit_loss;
    double win_rate;
    double largest_win;
    double largest_loss;
    double avg_win;
    double avg_loss;
    double profit_factor;  // Gross profit / Gross loss
    double sharpe_ratio;
    double max_drawdown;
    
    Statistics() : total_trades(0), winning_trades(0), losing_trades(0),
                   total_profit_loss(0), win_rate(0), largest_win(0), 
                   largest_loss(0), avg_win(0), avg_loss(0), 
                   profit_factor(0), sharpe_ratio(0), max_drawdown(0) {}
};

// Structure for incremental moving average tracking
struct MovingAverageState {
    double current_value;
    int period;
    double sum;
    
    MovingAverageState(int p = 0) : current_value(0), period(p), sum(0) {}
};

}

#endif