#pragma once

#include "Events.hpp"
#include <vector>

class Portfolio {
public:
    Portfolio(double initial_cash, int max_abs_position);

    bool pre_trade_check(const Order& order) const;
    void on_trade(const Trade& trade);

    int position() const;
    double cash() const;
    double realized_pnl() const;
    double equity(double mark_price) const;
    double total_pnl(double mark_price) const;

    int trade_count() const;
    int rejected_risk_count() const;
    int rejected_liquidity_count() const;

    void add_risk_reject();
    void add_liquidity_reject();

private:
    double initial_cash_;
    double cash_;
    int position_{0};
    double average_cost_{0.0};
    double realized_pnl_{0.0};

    int max_abs_position_;
    int trade_count_{0};
    int rejected_risk_count_{0};
    int rejected_liquidity_count_{0};
};