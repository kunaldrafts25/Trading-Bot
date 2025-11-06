#ifndef RISK_MANAGER_H
#define RISK_MANAGER_H

#include "Types.h"

namespace TradingBot {
class RiskManager {
public:
    RiskManager(double risk_percentage);
    int calculatePositionSize(double current_price, double available_balance); //calculate position size
    bool isTradeAllowed(double current_exposure, double balance); // check if tade is allowed
    double getMaxPositionValue(double balance) const;   //get max position val
private:
    double risk_percentage_;  // percentage of balance to risk per trade
    double max_exposure_;     // maximum percentage of balance in open positions
};
}
#endif 
