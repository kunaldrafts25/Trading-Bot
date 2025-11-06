#include "RiskManager.h"
#include <algorithm>

namespace TradingBot {

RiskManager::RiskManager(double risk_percentage)
    : risk_percentage_(risk_percentage), max_exposure_(80.0) {}
    
int RiskManager::calculatePositionSize(double current_price, double available_balance) {
    double risk_amount = available_balance * (risk_percentage_ / 100.0);// trade per capital
    int quantity = static_cast<int>(risk_amount / current_price); // position size
    double max_position_value = getMaxPositionValue(available_balance); // dont exceed max exposure
    int max_quantity = static_cast<int>(max_position_value / current_price);    
    return std::min(quantity, max_quantity);
}
bool RiskManager::isTradeAllowed(double current_exposure, double balance) {
    double exposure_percentage = (current_exposure / balance) * 100.0; //current exposure limits
    return exposure_percentage < max_exposure_;
}
double RiskManager::getMaxPositionValue(double balance) const {
    return balance * (max_exposure_ / 100.0); //max investable value
}
} 
