
#pragma once

#include "Events.hpp"
#include "CsvWriter.hpp"

class BacktestLogger {
public:
    BacktestLogger(const std::string& market_data_path,
                   const std::string& trades_path,
                   const std::string& equity_path);

    void log_market_data(const MarketData& md);
    void log_trade(const Trade& tr, int position_after, double cash_after, double equity_after);
    void log_equity(std::int64_t timestamp, double equity, double pnl, int position);

private:
    CsvWriter market_writer_;
    CsvWriter trades_writer_;
    CsvWriter equity_writer_;
};