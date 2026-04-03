#include "OrderBook.hpp"

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <limits>

void OrderBook::sort_books() {
    std::sort(bids_.begin(), bids_.end(), [](const RestingOrder& a, const RestingOrder& b) {
        if (a.price != b.price) {
            return a.price > b.price;
        }
        return a.timestamp < b.timestamp;
    });

    std::sort(asks_.begin(), asks_.end(), [](const RestingOrder& a, const RestingOrder& b) {
        if (a.price != b.price) {
            return a.price < b.price;
        }
        return a.timestamp < b.timestamp;
    });
}

bool OrderBook::order_exists(std::uint64_t order_id) const {
    auto pred = [order_id](const RestingOrder& o) { return o.order_id == order_id; };
    return std::any_of(bids_.begin(), bids_.end(), pred) || std::any_of(asks_.begin(), asks_.end(), pred);
}

void OrderBook::on_add(const OrderAdd& event) {
    if (order_exists(event.order_id)) {
        throw std::runtime_error("Duplicate order_id in OrderBook: " + std::to_string(event.order_id));
    }

    RestingOrder ro;
    ro.order_id = event.order_id;
    ro.timestamp = event.timestamp;
    ro.side = event.side;
    ro.price = event.price;
    ro.quantity = event.quantity;

    if (event.side == Side::Bid) {
        bids_.push_back(ro);
    } else {
        asks_.push_back(ro);
    }

    sort_books();
}

void OrderBook::on_remove(const OrderRemove& event) {
    auto remove_id = [id = event.order_id](std::vector<RestingOrder>& book) {
        auto it = std::remove_if(book.begin(), book.end(), [id](const RestingOrder& o) {
            return o.order_id == id;
        });
        const bool removed = (it != book.end());
        book.erase(it, book.end());
        return removed;
    };

    const bool removed_bid = remove_id(bids_);
    const bool removed_ask = remove_id(asks_);

    if (!removed_bid && !removed_ask) {
        std::cout << "[ORDERBOOK][WARN] REMOVE ignored for unknown/already filled order_id="
                  << event.order_id << "\n";
    }
}

double OrderBook::best_bid() const {
    if (bids_.empty()) {
        return 0.0;
    }
    return bids_.front().price;
}

double OrderBook::best_ask() const {
    if (asks_.empty()) {
        return 0.0;
    }
    return asks_.front().price;
}

double OrderBook::spread() const {
    if (bids_.empty() || asks_.empty()) {
        return 0.0;
    }
    return best_ask() - best_bid();
}

MarketData OrderBook::snapshot(std::int64_t ts) const {
    MarketData md;
    md.timestamp = ts;
    md.best_bid = best_bid();
    md.best_ask = best_ask();

    if (last_trade_price_ > 0.0) {
        md.last_price = last_trade_price_;
    } else if (md.best_bid > 0.0 && md.best_ask > 0.0) {
        md.last_price = 0.5 * (md.best_bid + md.best_ask);
    } else {
        md.last_price = 0.0;
    }

    md.volume = last_trade_volume_;
    return md;
}

void OrderBook::display(int levels) const {
    std::cout << "\n========== ORDER BOOK ==========\n";
    std::cout << "ASKS\n";
    for (int i = 0; i < std::min<int>(levels, static_cast<int>(asks_.size())); ++i) {
        std::cout << std::setw(8) << asks_[i].order_id
                  << " | " << std::setw(10) << std::fixed << std::setprecision(2) << asks_[i].price
                  << " | " << std::setw(6) << asks_[i].quantity << "\n";
    }

    std::cout << "BIDS\n";
    for (int i = 0; i < std::min<int>(levels, static_cast<int>(bids_.size())); ++i) {
        std::cout << std::setw(8) << bids_[i].order_id
                  << " | " << std::setw(10) << std::fixed << std::setprecision(2) << bids_[i].price
                  << " | " << std::setw(6) << bids_[i].quantity << "\n";
    }
    std::cout << "================================\n";
}

const std::vector<RestingOrder>& OrderBook::bids() const {
    return bids_;
}

const std::vector<RestingOrder>& OrderBook::asks() const {
    return asks_;
}

std::vector<RestingOrder>& OrderBook::bids_mutable() {
    return bids_;
}

std::vector<RestingOrder>& OrderBook::asks_mutable() {
    return asks_;
}

void OrderBook::set_last_trade(double price, std::int64_t volume) {
    last_trade_price_ = price;
    last_trade_volume_ = volume;
}