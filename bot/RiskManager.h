#ifndef RISK_MANAGER_H
#define RISK_MANAGER_H

#include "Types.h"

namespace TradingBot {

class RiskManager {
public:
    RiskManager(double risk_percentage, double max_exposure);
    
    // Calculate position size based on available capital and max exposure
    int calculatePositionSize(double current_price, double available_balance);
    
    // Check if trade is allowed based on exposure limits
    bool isTradeAllowed(double current_exposure, double balance, std::string& reason);
    
    // Get maximum position value allowed
    double getMaxPositionValue(double balance) const;
    
    // Calculate risk amount per trade
    double getRiskAmount(double balance) const;
    
private:
    double risk_percentage_;  // Percentage of balance to risk per trade (for position sizing)
    double max_exposure_;     // Maximum percentage of balance in open positions
};

}

#endif