#pragma once

#include "Types.hpp"
#include <variant>
#include <optional>
#include <string>
#include <cstdint>

struct OrderAdd {
    std::int64_t timestamp{};
    std::uint64_t order_id{};
    Side side{};
    double price{};
    int quantity{};
};

struct OrderRemove {
    std::int64_t timestamp{};
    std::uint64_t order_id{};
};

using FeedEvent = std::variant<OrderAdd, OrderRemove>;

struct MarketData {
    std::int64_t timestamp{};
    double best_bid{};
    double best_ask{};
    double last_price{};
    std::int64_t volume{};
};

struct Order {
    std::uint64_t order_id{};
    std::int64_t timestamp{};
    Side side{};
    OrderType type{};
    double price{};
    int quantity{};
    std::string strategy_name{};
};

struct Trade {
    std::uint64_t trade_id{};
    std::int64_t timestamp{};
    std::uint64_t resting_order_id{};
    std::uint64_t aggressor_order_id{};
    Side aggressor_side{};
    double price{};
    int quantity{};
    std::string strategy_name{};
};