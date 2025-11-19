#ifndef BROKER_SIMULATOR_H
#define BROKER_SIMULATOR_H

#include <map>
#include <string>
#include "Types.h"

namespace TradingBot {

// Represents an open position
struct Position {
    std::string symbol;
    int quantity;
    double entry_price;
    std::string entry_date;
    
    Position() : quantity(0), entry_price(0) {}
    
    bool isOpen() const { return quantity > 0; }
};

class BrokerSimulator {
public:
    BrokerSimulator(double initial_balance);
    
    bool executeBuy(const std::string& symbol, int quantity, double price, 
                   const std::string& date, const std::string& reason = "");
    
    bool executeSell(const std::string& symbol, int quantity, double price, 
                    const std::string& date, const std::string& reason = "");
    
    double getBalance() const { return balance_; }
    double getInitialBalance() const { return initial_balance_; }
    
    Position getPosition(const std::string& symbol) const;
    bool hasPosition(const std::string& symbol) const;
    
    double getPortfolioValue(const std::string& symbol, double current_price) const;
    double getTotalEquity(const std::string& symbol, double current_price) const;
    
    const Trade& getLastTrade() const { return last_trade_; }
    
    // Get unrealized P/L for open position
    double getUnrealizedPL(const std::string& symbol, double current_price) const;
    
private:
    double balance_;
    double initial_balance_;
    std::map<std::string, Position> positions_;
    Trade last_trade_;
};

}

#endif