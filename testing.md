# Testing Guide for Nifty 50 Trading Bot

## Manual Testing Checklist

### 1. Configuration Validation Testing

#### Test Case 1.1: Valid Configuration
```bash
# Edit config.txt with valid values
./trading_bot --backtest
```
**Expected**: Bot runs successfully with all parameters loaded

#### Test Case 1.2: Invalid Risk Percentage
```ini
risk_percentage=150.0  # Invalid (>100%)
```
**Expected**: Error message and validation failure

#### Test Case 1.3: Invalid MA Periods
```ini
short_ma_period=50
long_ma_period=20  # Invalid (short >= long)
```
**Expected**: Error message about MA period mismatch

#### Test Case 1.4: Missing Config File
```bash
./trading_bot --config nonexistent.txt
```
**Expected**: Warning about missing file, uses defaults

### 2. Data Loading Testing

#### Test Case 2.1: Valid CSV File
```bash
# With correct data/nifty50_data.csv
./trading_bot --backtest
```
**Expected**: Successfully loads all valid data points

#### Test Case 2.2: Missing CSV File
```ini
data_file_path=data/missing.csv
```
**Expected**: Clear error message about file not found

#### Test Case 2.3: Malformed CSV Data
Create test file with invalid data:
```csv
Date,Open,High,Low,Close,Volume
2024-01-01,invalid,21834.35,21680.85,21741.90,154000000
```
**Expected**: Skip invalid lines with warning messages

#### Test Case 2.4: OHLC Validation
Create test file with invalid OHLC:
```csv
Date,Open,High,Low,Close,Volume
2024-01-01,21800,21700,21850,21750,154000000
# High < Low - invalid
```
**Expected**: Validation error and line skipped

### 3. Strategy Testing

#### Test Case 3.1: Golden Cross Detection
```bash
# Monitor logs for Golden Cross signals
./trading_bot --backtest | grep "Golden Cross"
```
**Expected**: BUY signals when short MA crosses above long MA

#### Test Case 3.2: Death Cross Detection
```bash
./trading_bot --backtest | grep "Death Cross"
```
**Expected**: SELL signals when short MA crosses below long MA

#### Test Case 3.3: RSI Filter
```ini
# Use very short MA periods to generate frequent signals
short_ma_period=5
long_ma_period=10
```
**Expected**: Some signals filtered out due to RSI > 70 or < 30

#### Test Case 3.4: Stop Loss Trigger
```ini
stop_loss_percentage=3.0  # Very tight stop
```
**Expected**: Positions closed with "Stop loss triggered" reason

#### Test Case 3.5: Take Profit Trigger
```ini
take_profit_percentage=5.0  # Low target
```
**Expected**: Positions closed with "Take profit triggered" reason

### 4. Risk Management Testing

#### Test Case 4.1: Position Sizing
```ini
risk_percentage=10.0
initial_balance=100000
```
**Expected**: Each position ≈ 10,000 INR (10% of balance)

#### Test Case 4.2: Maximum Exposure
```ini
max_exposure=50.0  # Only 50% can be invested
```
**Expected**: Large position requests limited to 50% of balance

#### Test Case 4.3: Insufficient Balance
```ini
initial_balance=1000  # Very small
```
**Expected**: Cannot execute trades due to insufficient funds

### 5. Broker Simulator Testing

#### Test Case 5.1: Buy Execution
**Setup**: Run with valid config
**Check**: 
- Balance decreases by (quantity × price)
- Position created with correct entry price
- Trade logged

#### Test Case 5.2: Sell Execution
**Setup**: Execute buy, then sell
**Check**:
- Balance increases by (quantity × price)
- P/L calculated correctly
- Position closed if all shares sold

#### Test Case 5.3: Averaging Down
**Setup**: Execute multiple buys
**Check**:
- Position quantity accumulates
- Entry price averages correctly
- Example: Buy 10 @ 100, Buy 10 @ 120 → Avg entry = 110

#### Test Case 5.4: Partial Sell
**Setup**: Buy 100 shares, Sell 50 shares
**Check**:
- 50 shares remain in position
- Balance updated correctly
- P/L calculated only for sold shares

### 6. Live Simulation Testing

#### Test Case 6.1: Data Generation
```bash
./trading_bot --live
```
**Check**:
- Prices change randomly but realistically
- OHLC constraints maintained (H ≥ O,C,L)
- Volume varies reasonably

#### Test Case 6.2: Real-time Execution
**Check**:
- Delay between iterations (500ms default)
- Signals generated on new data
- Portfolio value updates

### 7. Trade Logging Testing

#### Test Case 7.1: CSV File Creation
```bash
./trading_bot --backtest
ls -la logs/trades.csv
```
**Expected**: File created with header and trade records

#### Test Case 7.2: Trade Data Completeness
**Check CSV contains**:
- All executed trades
- Correct timestamps
- Accurate prices and quantities
- P/L for sell trades
- Trade reasons in quotes

#### Test Case 7.3: Statistics Calculation
**Check output for**:
- Win rate matches trade count
- Total P/L equals final balance - initial balance
- Largest win/loss identified correctly
- Profit factor = total profit / total loss

### 8. Performance Metrics Testing

#### Test Case 8.1: Sharpe Ratio
**Setup**: Run long backtest
**Check**: 
- Sharpe ratio calculated (should be < 3 for realistic strategies)
- Higher Sharpe = better risk-adjusted returns

#### Test Case 8.2: Maximum Drawdown
**Setup**: Run backtest through volatile period
**Check**:
- Max drawdown percentage reported
- Should be > 0 if any losing periods occurred

#### Test Case 8.3: Profit Factor
**Setup**: Run complete backtest
**Check**:
- Profit factor > 1 means profitable
- Profit factor < 1 means losing strategy

### 9. Edge Cases Testing

#### Test Case 9.1: No Trades Executed
```ini
short_ma_period=200
long_ma_period=250
```
**Expected**: No crossovers occur, HOLD throughout, stats show 0 trades

#### Test Case 9.2: First Trade Is Sell
**Setup**: Start with data already in death cross
**Expected**: SELL signal ignored (no position to sell)

#### Test Case 9.3: Data Ends During Open Position
**Expected**: Position closed at market close with final price

#### Test Case 9.4: Extremely Small Balance
```ini
initial_balance=100
```
**Expected**: Cannot buy even 1 share if price > 100

#### Test Case 9.5: Zero Volume Data
**Setup**: CSV with volume=0
**Expected**: Data loaded but may affect live simulation

### 10. Integration Testing

#### Test Case 10.1: Full Backtest Run
```bash
time ./trading_bot --backtest
```
**Check**:
- Completes without crashes
- All trades logged
- Statistics calculated
- Performance reasonable (< 10 seconds for 365 days)

#### Test Case 10.2: Full Live Simulation
```bash
./trading_bot --live
```
**Check**:
- Runs for specified iterations
- Real-time feel with delays
- Clean exit after completion

#### Test Case 10.3: Configuration Reload
```bash
# Edit config.txt while NOT running
./trading_bot --backtest
# Edit again
./trading_bot --backtest
```
**Expected**: New configuration loaded each time

## Automated Testing Strategy

### Unit Tests to Implement

```cpp
// Test SMA calculation
void testSMACalculation() {
    std::vector<OHLCV> data = {
        {.close = 100}, {.close = 110}, {.close = 120}
    };
    Strategy s(2, 3);
    assert(s.calculateSMA(data, 2, 2) == 115.0);  // (110+120)/2
}

// Test RSI calculation
void testRSICalculation() {
    // Known RSI values from sample data
    // Verify against financial calculator
}

// Test position sizing
void testPositionSizing() {
    RiskManager rm(10.0, 80.0);
    int qty = rm.calculatePositionSize(100.0, 10000.0);
    assert(qty == 10);  // 10% of 10000 / 100 per share
}

// Test P/L calculation
void testProfitLoss() {
    BrokerSimulator broker(100000);
    broker.executeBuy("TEST", 10, 100, "2024-01-01");
    broker.executeSell("TEST", 10, 110, "2024-01-02");
    assert(broker.getLastTrade().profit_loss == 100.0);
}
```

### Performance Tests

```bash
# Test with large dataset
# Generate 10 years of data (3650 rows)
./trading_bot --backtest
# Should complete in < 30 seconds

# Memory leak test
valgrind --leak-check=full ./trading_bot --backtest
# Should show no leaks

# Profile CPU usage
perf record ./trading_bot --backtest
perf report
```

## Regression Testing

After any code changes, run these critical tests:

1. **Baseline Backtest**: Run with known config and verify same results
2. **Trade Count**: Ensure same number of trades executed
3. **Final Balance**: Verify final balance matches previous run
4. **Statistics**: Check all metrics remain consistent

### Creating Test Baseline
```bash
# First run to establish baseline
./trading_bot --backtest > baseline_output.txt
grep "Final Balance" baseline_output.txt

# After changes
./trading_bot --backtest > new_output.txt
diff baseline_output.txt new_output.txt
```

## Common Issues and Solutions

### Issue 1: No Trades Executed
**Causes**:
- MA periods too long for data length
- RSI filter too restrictive
- Insufficient balance

**Solution**: Check indicator values, reduce MA periods, increase balance

### Issue 2: Too Many Trades
**Causes**:
- MA periods too short (noise)
- No RSI filter

**Solution**: Increase MA periods, enable RSI filter

### Issue 3: Large Losses
**Causes**:
- No stop loss
- Poor parameter choices
- Unfavorable market conditions

**Solution**: Enable stop loss, optimize parameters, test on different periods

### Issue 4: Compilation Errors
**Causes**:
- Missing C++17 support
- Missing headers

**Solution**: Use `-std=c++17` flag, install build-essential

### Issue 5: File Not Found
**Causes**:
- Wrong working directory
- Incorrect path in config

**Solution**: Run from correct directory or use absolute paths

## Test Data Validation

### Verify Test Data Quality
```bash
# Check for missing dates
# Check for price anomalies (gaps > 5%)
# Check for volume outliers
# Verify chronological order
```

### Generate Test Datasets
```python
# Create synthetic test data
import pandas as pd
import numpy as np

dates = pd.date_range('2024-01-01', periods=365)
prices = 20000 + np.cumsum(np.random.randn(365) * 100)
data = pd.DataFrame({
    'Date': dates,
    'Open': prices,
    'High': prices * 1.01,
    'Low': prices * 0.99,
    'Close': prices,
    'Volume': 200000000 + np.random.randint(-50000000, 50000000, 365)
})
data.to_csv('test_data.csv', index=False)
```

## Continuous Testing

### Pre-commit Checks
1. Compile with warnings: `g++ -Wall -Wextra -Werror`
2. Run basic backtest
3. Check logs directory created
4. Verify no segfaults

### Pre-release Checklist
- [ ] All test cases pass
- [ ] No compiler warnings
- [ ] Documentation updated
- [ ] Example config validated
- [ ] Performance benchmarks met
- [ ] Memory leaks checked

---

**Note**: Testing trading systems is critical. Always validate results manually and understand why certain trades occur before deploying any strategy.