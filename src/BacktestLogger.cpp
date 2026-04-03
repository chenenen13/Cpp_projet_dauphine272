#include "BacktestLogger.hpp"

#include <string>

BacktestLogger::BacktestLogger(const std::string& market_data_path,
                               const std::string& trades_path,
                               const std::string& equity_path)
    : market_writer_(market_data_path),
      trades_writer_(trades_path),
      equity_writer_(equity_path) {
    market_writer_.write_row({"timestamp", "best_bid", "best_ask", "last_price", "volume"});
    trades_writer_.write_row({"trade_id", "timestamp", "resting_order_id", "aggressor_order_id",
                              "aggressor_side", "price", "quantity", "strategy_name",
                              "position_after", "cash_after", "equity_after"});
    equity_writer_.write_row({"timestamp", "equity", "pnl", "position"});
}

void BacktestLogger::log_market_data(const MarketData& md) {
    market_writer_.write_row({
        std::to_string(md.timestamp),
        std::to_string(md.best_bid),
        std::to_string(md.best_ask),
        std::to_string(md.last_price),
        std::to_string(md.volume)
    });
}

void BacktestLogger::log_trade(const Trade& tr, int position_after, double cash_after, double equity_after) {
    market_writer_.is_open();
    trades_writer_.write_row({
        std::to_string(tr.trade_id),
        std::to_string(tr.timestamp),
        std::to_string(tr.resting_order_id),
        std::to_string(tr.aggressor_order_id),
        (tr.aggressor_side == Side::Bid ? "BID" : "ASK"),
        std::to_string(tr.price),
        std::to_string(tr.quantity),
        tr.strategy_name,
        std::to_string(position_after),
        std::to_string(cash_after),
        std::to_string(equity_after)
    });
}

void BacktestLogger::log_equity(std::int64_t timestamp, double equity, double pnl, int position) {
    equity_writer_.write_row({
        std::to_string(timestamp),
        std::to_string(equity),
        std::to_string(pnl),
        std::to_string(position)
    });
}