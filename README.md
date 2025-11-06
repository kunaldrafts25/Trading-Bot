# 📉 **Trading-Bot**

> ⚙️ *A lightweight C++ trading bot for backtesting and simulating trading strategies on the Nifty 50 index (or any suitable OHLCV data).*

---

## 🚀 **Features**

* 🧠 **Market Data Handler** – Loads historical OHLCV data from CSV.
* 📈 **Strategy Module** – Implements a *Moving Average Crossover* strategy combined with an *RSI filter*.
* 💰 **Risk Manager** – Controls position sizing and exposure limits.
* 🏦 **Broker Simulator** – Simulates order execution and portfolio management.
* 🦾 **Trade Logger** – Logs executed trades and computes performance statistics.
* 🔁 **Dual Mode** – Supports both **Backtest** and **Live Simulation** modes.

---

## 📁 **Project Structure**

```
Trading-Bot/
├── bot/                    # Core source files and headers
│   ├── main.cpp
│   ├── Config.cpp / Config.h
│   ├── MarketDataHandler.cpp / MarketDataHandler.h
│   ├── Strategy.cpp / Strategy.h
│   ├── RiskManager.cpp / RiskManager.h
│   ├── BrokerSimulator.cpp / BrokerSimulator.h
│   ├── TradeLogger.cpp / TradeLogger.h
│   └── Types.h
│
├── data/                   # Example data input
│   └── nifty50_data.csv
│
├── logs/                   # Generated output logs
│   └── trades.csv
│
├── config.txt              # Configuration file
├── README.md               # (this file)
└── .gitignore
```

---

## 🛠️ **Build & Run**

### **Requirements**

* 🧩 C++17 (or later) compiler (GCC, Clang, or MinGW/MSYS2).
* ⚡ Standard library support (no external dependencies).

---

### **Build & Execute**

#### 🪟 **Windows (PowerShell)**

```bash
g++ -std=c++17 *.cpp -o trading_bot.exe
.\trading_bot.exe            # Run in back-test mode
.\trading_bot.exe --live     # Run in live simulation mode
```

#### 🐧 **Linux / macOS**

```bash
g++ -std=c++17 *.cpp -o trading_bot
./trading_bot              # Back-test mode
./trading_bot --live       # Live simulation mode
```

💡 *You can also integrate this project with CMake or an IDE if preferred.*

---

## ⚙️ **Configuration (config.txt)**

Example:

```
initial_balance=100000
risk_percentage_per_trade=2
short_ma_period=10
long_ma_period=50
rsi_period=14
rsi_upper=70
rsi_lower=30
mode=backtest
data_filepath=data/nifty50_data.csv
```

---

## 📄 **Data Format**

Your CSV file should look like this:

```
Date,Open,High,Low,Close,Volume
2024-01-01,21500.50,21650.75,21480.25,21620.50,125000000
```

✅ Ensure:

* Chronologically sorted data
* No missing or corrupted entries

---

## 📊 **Trading Strategy**

**Moving Average Crossover + RSI Filter**

| Signal Type | Condition                                                                     |
| ----------- | ----------------------------------------------------------------------------- |
| 🟢 **BUY**  | Short MA crosses above Long MA *(Golden Cross)* **AND** RSI < Upper Threshold |
| 🔴 **SELL** | Short MA crosses below Long MA *(Death Cross)* **AND** RSI > Lower Threshold  |
| 🟡 **HOLD** | Neither condition met                                                         |

---

## ✅ **Why Use This Bot?**

* ⚡ **Fast:** Built in C++ for high performance.
* 🧩 **Customizable:** Plug in your own strategy or indicator.
* 🎓 **Educational:** Perfect for learning trading algorithms, backtesting, and portfolio control.
* 🚀 **Extensible:** Can be expanded to support multiple instruments, APIs, or real-time trading feeds.

---

## 🥪 **How to Extend**

💡 Ideas for next-level upgrades:

* 🔍 Replace **Strategy.cpp** with a momentum, mean-reversion, or ML-based algorithm.
* 🔗 Connect **BrokerSimulator** to a live broker API (like Zerodha, Fyers, etc.).
* 📡 Expand **MarketDataHandler** for streaming or multi-asset data.
* 📊 Add performance metrics like **Sharpe Ratio, Max Drawdown**, etc.
* 🥮 Implement **Portfolio Optimization** and **Event-Driven Architecture**.

---

## ⚠️ **Important Notes**

> ⚠️ **Disclaimer:**
> This repository is for **educational and development purposes only**.
> Past performance does **not guarantee future results**.

* Backtesting ≠ Real trading.
* Clean and unbiased data is critical.
* Always test thoroughly before live deployment.

---

## 🔧 **Prerequisites & Dependencies**

* ✅ C++17 (or newer)
* 📁 CSV data file in standard OHLCV format
* 🌐 Optional: API integration for live mode (to be implemented manually)

---

## 📚 **License & Contribution**

* 🔄 Feel free to fork, modify, and submit pull requests!
* 🐞 Report issues or suggest improvements in the GitHub *Issues* tab.

---

## 📬 **Contact & Support**

👤 **Kunal Singh**

📧 **Email:** [kunalsingh2514@gmail.com](mailto:kunalsingh2514@gmail.com)
🌐 **GitHub:** [github.com/kunaldrafts25](https://github.com/kunaldrafts25)
💬 **LinkedIn:** [linkedin.com/in/kunalsingh25](https://linkedin.com/in/kunalsinghh25)

---

