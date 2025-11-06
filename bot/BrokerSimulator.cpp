#include "BrokerSimulator.h"
#include <iostream>
#include <iomanip>

namespace TradingBot {
BrokerSimulator::BrokerSimulator(double initial_balance)
    : balance_(initial_balance), initial_balance_(initial_balance) {}
bool BrokerSimulator::executeBuy(const std::string& symbol, int quantity, 
                                  double price, const std::string& date) {
    double cost = quantity * price;
        if (cost > balance_) {
        std::cout << "Insufficient balance for buy order. Required: INR " << cost 
                  << ", Available: INR " << balance_ << std::endl;
        return false;
    }
    
    // Execute the buy
    balance_ -= cost;
    
    if (positions_.find(symbol) != positions_.end()) {
        Position& pos = positions_[symbol];
        double total_cost = (pos.quantity * pos.entry_price) + cost;
        pos.quantity += quantity;
        pos.entry_price = total_cost / pos.quantity;
    } else {
        Position pos;
        pos.symbol = symbol;
        pos.quantity = quantity;
        pos.entry_price = price;
        pos.entry_date = date;
        positions_[symbol] = pos;
    }
    
    // Record the trade
    last_trade_.timestamp = date;
    last_trade_.symbol = symbol;
    last_trade_.signal = Signal::BUY;
    last_trade_.price = price;
    last_trade_.quantity = quantity;
    last_trade_.value = cost;
    last_trade_.balance_after = balance_;
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "BUY executed: " << quantity << " shares @ INR " << price 
              << " | Total: INR " << cost << " | Balance: INR " << balance_ << std::endl;
    
    return true;
}

bool BrokerSimulator::executeSell(const std::string& symbol, int quantity, 
                                   double price, const std::string& date) {
    // Check if we have a position
    if (positions_.find(symbol) == positions_.end()) {
        std::cout << "No position to sell for " << symbol << std::endl;
        return false;
    }
    
    Position& pos = positions_[symbol];
    if (quantity > pos.quantity) {
        quantity = pos.quantity; 
    }
    double revenue = quantity * price;
    balance_ += revenue;
    
    // Calculate profit/loss
    double profit_loss = (price - pos.entry_price) * quantity;
    
    // Update position
    pos.quantity -= quantity;
    if (pos.quantity == 0) {
        positions_.erase(symbol);  
    }
    
    // Record the trade
    last_trade_.timestamp = date;
    last_trade_.symbol = symbol;
    last_trade_.signal = Signal::SELL;
    last_trade_.price = price;
    last_trade_.quantity = quantity;
    last_trade_.value = revenue;
    last_trade_.balance_after = balance_;    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "SELL executed: " << quantity << " shares @ INR " << price 
              << " | Total: INR " << revenue << " | P/L: INR " << profit_loss 
              << " | Balance: INR " << balance_ << std::endl;    
    return true;
}

Position BrokerSimulator::getPosition(const std::string& symbol) const {
    auto it = positions_.find(symbol);
    if (it != positions_.end()) {
        return it->second;
    }
    return Position();
}
bool BrokerSimulator::hasPosition(const std::string& symbol) const {
    return positions_.find(symbol) != positions_.end() && 
           positions_.at(symbol).quantity > 0;
}
double BrokerSimulator::getPortfolioValue(const std::string& symbol, double current_price) const {
    double portfolio_value = balance_;    
    if (hasPosition(symbol)) {
        Position pos = getPosition(symbol);
        portfolio_value += pos.quantity * current_price;
    }    
    return portfolio_value;
}

} 
