#ifndef BROKER_SIMULATOR_H
#define BROKER_SIMULATOR_H

#include <map>
#include <string>
#include "Types.h"

namespace TradingBot {

//This represent an open position
struct Position {
    std::string symbol;
    int quantity;
    double entry_price;
    std::string entry_date;
    
    Position() : quantity(0), entry_price(0) {}
};

class BrokerSimulator {
public:
    BrokerSimulator(double initial_balance);
    bool executeBuy(const std::string& symbol, int quantity, double price, const std::string& date); //buy order
    bool executeSell(const std::string& symbol, int quantity, double price, const std::string& date); //sell order
    double getBalance() const { return balance_; } //balance rn
    Position getPosition(const std::string& symbol) const; //current position
    bool hasPosition(const std::string& symbol) const; //open position
    double getPortfolioValue(const std::string& symbol, double current_price) const; //portfolio value
    const Trade& getLastTrade() const { return last_trade_; }    //last trade
private:
    double balance_;
    double initial_balance_;
    std::map<std::string, Position> positions_;
    Trade last_trade_;
};
} 

#endif
