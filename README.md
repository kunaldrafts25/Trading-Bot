# Nifty 50 Trading Bot

A C++ trading bot for backtesting and simulating trading strategies on Nifty 50 index data.

## Project Structure

```
nifty50-trading-bot/
├── bot/                    # source files and headers (moved into bot/)
│   ├── main.cpp
│   ├── Config.cpp
│   ├── Config.h
│   ├── MarketDataHandler.cpp
│   ├── MarketDataHandler.h
│   ├── Strategy.cpp
│   ├── Strategy.h
│   ├── RiskManager.cpp
│   ├── RiskManager.h
│   ├── BrokerSimulator.cpp
│   ├── BrokerSimulator.h
│   ├── TradeLogger.cpp
│   ├── TradeLogger.h
│   └── Types.h
├── data/
│   └── nifty50_data.csv
├── logs/
│   └── trades.csv (generated)
├── config.txt
├── README.md
└── .gitignore
```

## Features

- **Market Data Handler**: Loads historical OHLCV data from CSV
- **Trading Strategy**: Moving Average Crossover with RSI filter
- **Risk Management**: Position sizing and exposure limits
- **Broker Simulator**: Simulates order execution and portfolio management
- **Trade Logger**: Logs all trades and generates statistics
- **Dual Mode**: Backtest mode and live simulation mode

## Build & Run (direct compile)

This project can be compiled directly with g++ without using CMake. The examples below assume you have a working g++ toolchain installed (MinGW/MSYS2 on Windows or GCC on Linux/macOS).

Windows (PowerShell):

```powershell
g++ -std=c++17 *.cpp -o trading_bot.exe
.\trading_bot.exe            # run (backtest mode)
.\trading_bot.exe --live     # run live simulation mode
```

Linux / macOS:

```bash
g++ -std=c++17 *.cpp -o trading_bot
./trading_bot
./trading_bot --live
```

Notes:
- Use `-std=c++17` (or newer) to ensure <thread> and other C++ standard library features are available.
- If you prefer an IDE or CMake+Visual Studio workflow, the CMake files in the repo can still be used, but the simplest route is the direct compile above.

## Configuration

Edit `config.txt` to modify:
- Initial balance
- Risk percentage per trade
- Moving average periods
- Data file path
- Trading mode

## Data Format

CSV file should have format:
```
Date,Open,High,Low,Close,Volume
2024-01-01,21500.50,21650.75,21480.25,21620.50,125000000
```

## Trading Strategy

The bot uses a **Moving Average Crossover** strategy:
- **BUY Signal**: When short MA crosses above long MA (Golden Cross) and RSI < 70
- **SELL Signal**: When short MA crosses below long MA (Death Cross) and RSI > 30
- **HOLD**: Otherwise

