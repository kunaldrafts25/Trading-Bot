#include "BrokerSimulator.h"
#include <iostream>
#include <iomanip>

namespace TradingBot {

BrokerSimulator::BrokerSimulator(double initial_balance)
    : balance_(initial_balance), initial_balance_(initial_balance) {}

bool BrokerSimulator::executeBuy(const std::string& symbol, int quantity, 
                                 double price, const std::string& date,
                                 const std::string& reason) {
    if (quantity <= 0) {
        std::cout << "Invalid quantity for buy order: " << quantity << std::endl;
        return false;
    }
    
    double cost = quantity * price;
    
    if (cost > balance_) {
        std::cout << "Insufficient balance for buy order. Required: INR " << cost 
                  << ", Available: INR " << balance_ << std::endl;
        return false;
    }
    
    // Execute the buy
    balance_ -= cost;
    
    // Update or create position
    if (positions_.find(symbol) != positions_.end()) {
        Position pos = positions_[symbol];  // Copy position
        double total_cost = (pos.quantity * pos.entry_price) + cost;
        pos.quantity += quantity;
        pos.entry_price = total_cost / pos.quantity;  // Average entry price
        positions_[symbol] = pos;
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
    last_trade_.profit_loss = 0;  // No P/L on entry
    last_trade_.reason = reason;
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "[" << date << "] BUY executed: " << quantity 
              << " shares @ INR " << price 
              << " | Total: INR " << cost 
              << " | Balance: INR " << balance_;
    if (!reason.empty()) {
        std::cout << " | Reason: " << reason;
    }
    std::cout << std::endl;
    
    return true;
}

bool BrokerSimulator::executeSell(const std::string& symbol, int quantity, 
                                  double price, const std::string& date,
                                  const std::string& reason) {
    // Check if we have a position
    if (positions_.find(symbol) == positions_.end()) {
        std::cout << "No position to sell for " << symbol << std::endl;
        return false;
    }
    
    Position pos = positions_[symbol];  // Copy position
    
    if (pos.quantity <= 0) {
        std::cout << "No shares to sell for " << symbol << std::endl;
        return false;
    }
    
    if (quantity > pos.quantity) {
        std::cout << "Requested quantity (" << quantity 
                  << ") exceeds position size (" << pos.quantity 
                  << "). Selling all shares." << std::endl;
        quantity = pos.quantity;
    }
    
    double revenue = quantity * price;
    balance_ += revenue;
    
    // Calculate profit/loss
    double profit_loss = (price - pos.entry_price) * quantity;
    double profit_loss_pct = ((price - pos.entry_price) / pos.entry_price) * 100.0;
    
    // Update position
    pos.quantity -= quantity;
    if (pos.quantity == 0) {
        positions_.erase(symbol);
    } else {
        positions_[symbol] = pos;
    }
    
    // Record the trade
    last_trade_.timestamp = date;
    last_trade_.symbol = symbol;
    last_trade_.signal = Signal::SELL;
    last_trade_.price = price;
    last_trade_.quantity = quantity;
    last_trade_.value = revenue;
    last_trade_.balance_after = balance_;
    last_trade_.profit_loss = profit_loss;
    last_trade_.reason = reason;
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "[" << date << "] SELL executed: " << quantity 
              << " shares @ INR " << price 
              << " | Total: INR " << revenue 
              << " | P/L: INR " << profit_loss 
              << " (" << profit_loss_pct << "%)"
              << " | Balance: INR " << balance_;
    if (!reason.empty()) {
        std::cout << " | Reason: " << reason;
    }
    std::cout << std::endl;
    
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
    auto it = positions_.find(symbol);
    return it != positions_.end() && it->second.quantity > 0;
}

double BrokerSimulator::getPortfolioValue(const std::string& symbol, 
                                          double current_price) const {
    double portfolio_value = balance_;
    
    if (hasPosition(symbol)) {
        Position pos = getPosition(symbol);
        portfolio_value += pos.quantity * current_price;
    }
    
    return portfolio_value;
}

double BrokerSimulator::getTotalEquity(const std::string& symbol, 
                                       double current_price) const {
    return getPortfolioValue(symbol, current_price);
}

double BrokerSimulator::getUnrealizedPL(const std::string& symbol, 
                                        double current_price) const {
    if (!hasPosition(symbol)) {
        return 0.0;
    }
    
    Position pos = getPosition(symbol);
    return (current_price - pos.entry_price) * pos.quantity;
}

}