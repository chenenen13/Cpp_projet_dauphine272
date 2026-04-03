#pragma once

#include "OrderBook.hpp"
#include <vector>

class MatchingEngine {
public:
    explicit MatchingEngine(OrderBook& book);

    std::vector<Trade> match(const Order& order);

private:
    OrderBook& book_;
    std::uint64_t next_trade_id_{1};

    bool can_fully_fill(const Order& order) const;
};