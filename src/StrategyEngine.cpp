#include "StrategyEngine.hpp"

StrategyEngine::StrategyEngine(std::unique_ptr<Strategy> strategy)
    : strategy_(std::move(strategy)) {
    if (!strategy_) {
        throw std::runtime_error("StrategyEngine requires a valid strategy");
    }
}

Signal StrategyEngine::on_market_data(const MarketData& data) {
    return strategy_->on_market_data(data);
}

std::optional<Order> StrategyEngine::maybe_create_order(const MarketData& data, std::int64_t timestamp) {
    const Signal signal = on_market_data(data);

    if (signal == Signal::Hold) {
        last_signal_ = signal;
        return std::nullopt;
    }

    Order order;
    order.order_id = next_order_id_++;
    order.timestamp = timestamp;
    order.side = (signal == Signal::Buy ? Side::Bid : Side::Ask);
    order.type = strategy_->preferred_order_type();
    order.quantity = strategy_->quantity();
    order.strategy_name = strategy_->name();

    if (order.type == OrderType::Limit) {
        order.price = (order.side == Side::Bid ? data.best_ask : data.best_bid);
        if (order.price <= 0.0) {
            return std::nullopt;
        }
    } else {
        order.price = 0.0;
    }

    last_signal_ = signal;
    return order;
}

const Strategy& StrategyEngine::strategy() const {
    return *strategy_;
}