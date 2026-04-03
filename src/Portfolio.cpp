#include "Portfolio.hpp"

#include <algorithm>
#include <cmath>

Portfolio::Portfolio(double initial_cash, int max_abs_position)
    : initial_cash_(initial_cash),
      cash_(initial_cash),
      max_abs_position_(max_abs_position) {}

bool Portfolio::pre_trade_check(const Order& order) const {
    const int signed_qty = (order.side == Side::Bid ? order.quantity : -order.quantity);
    const int projected = position_ + signed_qty;
    return std::abs(projected) <= max_abs_position_;
}

void Portfolio::on_trade(const Trade& trade) {
    ++trade_count_;

    const int qty = trade.quantity;
    const double px = trade.price;

    if (trade.aggressor_side == Side::Bid) {
        cash_ -= px * qty;

        if (position_ >= 0) {
            const double total_cost = average_cost_ * position_ + px * qty;
            position_ += qty;
            average_cost_ = (position_ != 0) ? total_cost / position_ : 0.0;
        } else {
            const int cover_qty = std::min(qty, -position_);
            realized_pnl_ += (average_cost_ - px) * cover_qty;
            position_ += qty;

            if (position_ == 0) {
                average_cost_ = 0.0;
            } else if (position_ > 0) {
                average_cost_ = px;
            }
        }
    } else {
        cash_ += px * qty;

        if (position_ <= 0) {
            const double total_short_value = average_cost_ * (-position_) + px * qty;
            position_ -= qty;
            average_cost_ = (position_ != 0) ? total_short_value / (-position_) : 0.0;
        } else {
            const int sell_qty = std::min(qty, position_);
            realized_pnl_ += (px - average_cost_) * sell_qty;
            position_ -= qty;

            if (position_ == 0) {
                average_cost_ = 0.0;
            } else if (position_ < 0) {
                average_cost_ = px;
            }
        }
    }
}

int Portfolio::position() const {
    return position_;
}

double Portfolio::cash() const {
    return cash_;
}

double Portfolio::realized_pnl() const {
    return realized_pnl_;
}

double Portfolio::equity(double mark_price) const {
    return cash_ + position_ * mark_price;
}

double Portfolio::total_pnl(double mark_price) const {
    return equity(mark_price) - initial_cash_;
}

int Portfolio::trade_count() const {
    return trade_count_;
}

int Portfolio::rejected_risk_count() const {
    return rejected_risk_count_;
}

int Portfolio::rejected_liquidity_count() const {
    return rejected_liquidity_count_;
}

void Portfolio::add_risk_reject() {
    ++rejected_risk_count_;
}

void Portfolio::add_liquidity_reject() {
    ++rejected_liquidity_count_;
}