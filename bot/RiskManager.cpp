#include "RiskManager.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace TradingBot {

RiskManager::RiskManager(double risk_percentage, double max_exposure)
    : risk_percentage_(risk_percentage), max_exposure_(max_exposure) {}

int RiskManager::calculatePositionSize(double current_price, double available_balance) {
    // Calculate position size based on risk percentage
    // This represents how much capital to allocate, not risk of loss
    double position_value = available_balance * (risk_percentage_ / 100.0);
    
    // Ensure we don't exceed maximum exposure
    double max_position_value = getMaxPositionValue(available_balance);
    position_value = std::min(position_value, max_position_value);
    
    // Calculate quantity (rounded down to avoid over-allocation)
    int quantity = static_cast<int>(position_value / current_price);
    
    // Ensure at least 1 share if we have enough balance
    if (quantity == 0 && available_balance >= current_price) {
        quantity = 1;
    }
    
    return quantity;
}

bool RiskManager::isTradeAllowed(double current_exposure, double balance, 
                                std::string& reason) {
    if (balance <= 0) {
        reason = "Insufficient balance";
        return false;
    }
    
    double exposure_percentage = (current_exposure / balance) * 100.0;
    
    if (exposure_percentage >= max_exposure_) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        oss << "Maximum exposure limit reached (" << exposure_percentage 
            << "% >= " << max_exposure_ << "%)";
        reason = oss.str();
        return false;
    }
    
    return true;
}

double RiskManager::getMaxPositionValue(double balance) const {
    return balance * (max_exposure_ / 100.0);
}

double RiskManager::getRiskAmount(double balance) const {
    return balance * (risk_percentage_ / 100.0);
}

}