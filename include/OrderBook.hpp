#pragma once

#include "Events.hpp"
#include <vector>

struct RestingOrder {
    std::uint64_t order_id{};
    std::int64_t timestamp{};
    Side side{};
    double price{};
    int quantity{};
};

class OrderBook {
public:
    void on_add(const OrderAdd& event);
    void on_remove(const OrderRemove& event);

    double best_bid() const;
    double best_ask() const;
    double spread() const;

    MarketData snapshot(std::int64_t ts) const;
    void display(int levels) const;

    const std::vector<RestingOrder>& bids() const;
    const std::vector<RestingOrder>& asks() const;

    std::vector<RestingOrder>& bids_mutable();
    std::vector<RestingOrder>& asks_mutable();

    void set_last_trade(double price, std::int64_t volume);

private:
    std::vector<RestingOrder> bids_;
    std::vector<RestingOrder> asks_;

    double last_trade_price_{0.0};
    std::int64_t last_trade_volume_{0};

    void sort_books();
    bool order_exists(std::uint64_t order_id) const;
};