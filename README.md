# C++17 Simulated Trading Platform

## Project Overview

This project implements a **simulated trading platform in C++17** covering the full lifecycle of a trade:

**Signal → Execution → Post-trade Analysis**

The system is designed in a modular way to mimic the architecture of real electronic trading systems used in banks, hedge funds, and quantitative trading environments.

It includes:

- a **Market Data Feed** reading events from CSV
- a functional **Order Book**
- a **Matching Engine**
- a **Strategy Engine**
- a **Portfolio & Risk Manager**
- a **Reporting module**
- CSV export for analytics
- multiple trading strategies
- performance and risk metrics
- optional Python plots for visualization

---

## Main Objectives

The goal of the project is to simulate a realistic trading workflow:

1. The **MarketDataFeed** reads market events from a CSV file line by line
2. The **OrderBook** reconstructs the market state using `ADD` and `REMOVE` events
3. The **OrderBook** generates **MarketData snapshots**
4. The **StrategyEngine** receives market data and generates trading signals
5. Strategy orders are sent to the **MatchingEngine**
6. Executed trades update the **Portfolio**
7. The **Reporting** module computes post-trade performance metrics
8. Results are exported to CSV for analysis and plotting

---

## Architecture

### Core Modules

- **MarketDataFeed**
  - Reads CSV events
  - Emits `OrderAdd` and `OrderRemove`

- **OrderBook**
  - Maintains the state of the order book
  - Stores bids and asks
  - Applies price-time priority
  - Generates market snapshots

- **MatchingEngine**
  - Matches strategy orders against the current order book
  - Supports **Market** and **Limit** orders
  - Uses **all-or-nothing** execution policy for aggressive strategy orders

- **StrategyEngine**
  - Holds a strategy object through polymorphism
  - Receives market snapshots
  - Creates strategy orders from signals

- **Portfolio**
  - Tracks cash and inventory
  - Computes realized and total P&L
  - Applies pre-trade position limits
  - Tracks exposure

- **Reporting**
  - Tracks the equity curve
  - Computes risk and performance metrics
  - Displays final simulation statistics

- **BacktestLogger**
  - Exports:
    - market data snapshots
    - trades
    - equity curve

---

## C++17 Features Used

The project intentionally uses modern C++ features:

- `std::variant`
- `std::optional`
- `std::unique_ptr`
- inheritance and polymorphism
- templates (`RollingWindow<T>`)
- exception handling
- `std::filesystem`

---

## File Structure

```text
.
├── CMakeLists.txt
├── README.md
├── data/
│   ├── test_events.csv
│   └── test_events_long.csv
├── include/
│   ├── Types.hpp
│   ├── Events.hpp
│   ├── RollingWindow.hpp
│   ├── MarketDataFeed.hpp
│   ├── OrderBook.hpp
│   ├── MatchingEngine.hpp
│   ├── Strategy.hpp
│   ├── StrategyEngine.hpp
│   ├── Portfolio.hpp
│   ├── Reporting.hpp
│   ├── RiskMetrics.hpp
│   ├── CsvWriter.hpp
│   └── BacktestLogger.hpp
├── src/
│   ├── main.cpp
│   ├── MarketDataFeed.cpp
│   ├── OrderBook.cpp
│   ├── MatchingEngine.cpp
│   ├── Strategy.cpp
│   ├── StrategyEngine.cpp
│   ├── Portfolio.cpp
│   ├── Reporting.cpp
│   ├── RiskMetrics.cpp
│   ├── CsvWriter.cpp
│   └── BacktestLogger.cpp
├── output/
│   ├── market_data.csv
│   ├── trades.csv
│   ├── equity_curve.csv
│   └── risk_report.csv
└── scripts/
    └── plot_results.py


## Run the code 

mkdir -p build
cmake -S . -B build
cmake --build build

./build/trading_sim