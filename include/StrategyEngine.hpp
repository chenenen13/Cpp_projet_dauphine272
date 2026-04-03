#pragma once

#include "Strategy.hpp"
#include <memory>
#include <optional>
#include <cstdint>

class StrategyEngine {
public:
    explicit StrategyEngine(std::unique_ptr<Strategy> strategy);

    Signal on_market_data(const MarketData& data);
    std::optional<Order> maybe_create_order(const MarketData& data, std::int64_t timestamp);

    const Strategy& strategy() const;

private:
    std::unique_ptr<Strategy> strategy_;
    std::uint64_t next_order_id_{1000000};
    Signal last_signal_{Signal::Hold};
};