# 📉 **Trading-Bot**

A professional-grade C++ trading bot for backtesting and simulating trading strategies on Nifty 50 index data with comprehensive risk management and performance analytics.

## 🚀 Features

### Core Functionality
- **Market Data Handler**: Robust CSV parsing with validation and error handling
- **Advanced Trading Strategy**: Moving Average Crossover with RSI filter and risk controls
- **Risk Management**: 
  - Dynamic position sizing based on account balance
  - Maximum exposure limits
  - Stop-loss and take-profit orders
- **Broker Simulator**: 
  - Realistic order execution
  - Portfolio tracking with unrealized P/L
  - Average entry price calculation for multiple buys
- **Comprehensive Trade Logging**: 
  - Detailed trade history with reasons
  - Advanced performance metrics (Sharpe Ratio, Max Drawdown, Profit Factor)
  - CSV export for analysis
- **Dual Mode Operation**: 
  - Backtest on historical data
  - Live simulation with synthetic market data

### Performance Metrics
- Total/Winning/Losing trades
- Win rate percentage
- Average win/loss amounts
- Largest win/loss
- Profit factor (gross profit / gross loss)
- Maximum drawdown percentage
- Sharpe ratio (risk-adjusted returns)

## 📁 Project Structure

```
nifty50-trading-bot/
├── bot/                       # Source files
│   ├── main.cpp              # Main entry point with TradingEngine
│   ├── Config.cpp/h          # Configuration management with validation
│   ├── MarketDataHandler.cpp/h   # CSV data loading and live data generation
│   ├── Strategy.cpp/h        # Trading strategy with MA crossover and RSI
│   ├── RiskManager.cpp/h     # Position sizing and risk controls
│   ├── BrokerSimulator.cpp/h # Order execution and portfolio management
│   ├── TradeLogger.cpp/h     # Trade logging and statistics
│   └── Types.h               # Common data structures and enums
├── data/
│   └── nifty50_data.csv      # Historical OHLCV data
├── logs/                      # Generated log files
│   ├── trades.csv            # Backtest trade log
│   └── live_trades.csv       # Live simulation trade log
├── config.txt                 # Configuration file
├── README.md
└── .gitignore
```

## 🛠️ Build & Run

### Prerequisites
- C++17 compatible compiler (g++, clang, MSVC)
- Standard C++ library with threading support

### Quick Start - Direct Compilation

**Windows (PowerShell/CMD):**
```powershell
cd bot
g++ -std=c++17 -O2 *.cpp -o trading_bot.exe
.\trading_bot.exe              # Run backtest (default)
.\trading_bot.exe --live       # Run live simulation
```

**Linux / macOS:**
```bash
cd bot
g++ -std=c++17 -O2 *.cpp -o trading_bot
./trading_bot                  # Run backtest (default)
./trading_bot --live           # Run live simulation
```

### Command Line Options
```bash
./trading_bot                  # Use mode from config.txt
./trading_bot --backtest       # Force backtest mode
./trading_bot --live           # Force live simulation mode
./trading_bot --config custom.txt  # Use custom config file
```

### Build Optimization Flags
For production use, compile with optimizations:
```bash
g++ -std=c++17 -O3 -march=native -DNDEBUG *.cpp -o trading_bot
```

## ⚙️ Configuration

Edit `config.txt` to customize trading parameters:

### Capital Management
```ini
initial_balance=100000        # Starting capital in INR
risk_percentage=10.0          # Capital allocation per trade (%)
max_exposure=80.0             # Max portfolio exposure (%)
stop_loss_percentage=5.0      # Stop loss threshold (%)
take_profit_percentage=15.0   # Take profit target (%)
```

### Strategy Parameters
```ini
short_ma_period=20           # Short-term MA period (days)
long_ma_period=50            # Long-term MA period (days)
rsi_period=14                # RSI calculation period (days)
```

### Mode Selection
```ini
mode=backtest                # backtest or live
```

### Live Simulation Settings
```ini
live_iterations=100          # Number of ticks to simulate
live_history_size=200        # Historical data points for context
live_delay_ms=500           # Delay between ticks (ms)
```

## 📊 Data Format

CSV file must have the following format:
```csv
Date,Open,High,Low,Close,Volume
2024-01-01,21727.75,21834.35,21680.85,21741.90,154000000
2024-01-02,21751.35,21755.60,21555.65,21665.80,263710000
```

**Data Requirements:**
- Header row required
- Date format: YYYY-MM-DD (or any consistent format)
- Prices: Decimal numbers
- Volume: Integer
- OHLC validation: High ≥ Low, High ≥ Open, High ≥ Close, Low ≤ Open, Low ≤ Close

## 📈 Trading Strategy

### Moving Average Crossover with RSI Filter

**BUY Signal:**
- Short MA crosses above Long MA (Golden Cross)
- RSI < 70 (not overbought)
- No existing position

**SELL Signal:**
- Short MA crosses below Long MA (Death Cross)
- RSI > 30 (not oversold)
- Has existing position

**Risk Management Exits:**
- Stop Loss: Position closed if loss exceeds configured percentage
- Take Profit: Position closed if profit reaches target percentage

**HOLD:**
- No crossover detected
- RSI in extreme zones
- Exposure limits reached

## 🎯 Key Improvements Over Original

### 1. **Fixed Critical Issues**
- ✅ Corrected risk management calculation (was allocating 40% per trade!)
- ✅ Fixed look-ahead bias in backtesting
- ✅ Improved CSV parsing with proper error handling
- ✅ Fixed position management edge cases

### 2. **Enhanced Features**
- ✅ Stop-loss and take-profit orders
- ✅ Better crossover detection (handles exact crosses)
- ✅ Detailed trade reasons in logs
- ✅ Unrealized P/L tracking for open positions
- ✅ Advanced performance metrics (Sharpe, Drawdown, Profit Factor)

### 3. **Code Quality**
- ✅ Comprehensive error handling with try-catch blocks
- ✅ Configuration validation
- ✅ Unified TradingEngine class (DRY principle)
- ✅ Better data validation
- ✅ Improved random number generation for live simulation

### 4. **User Experience**
- ✅ Detailed progress reporting
- ✅ Clear trade execution logs with reasons
- ✅ Professional summary statistics
- ✅ Better configuration with comments
- ✅ Command-line argument support

## 📝 Output Examples

### Trade Execution
```
[2024-03-18] SELL executed: 2 shares @ INR 22055.70 | Total: INR 44111.40 | 
P/L: INR 568.00 (1.30%) | Balance: INR 100568.00 | 
Reason: Death Cross detected (MA20=21890.50 < MA50=21920.30, RSI=62.50)
```

### Statistics Summary
```

---

**Disclaimer**: This software is for educational and research purposes only. It should not be used for actual trading without proper testing, risk assessment, and professional financial advice.