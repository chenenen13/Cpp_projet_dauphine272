#pragma once

#include <cstdint>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cctype>

enum class Side {
    Bid,
    Ask
};

enum class OrderType {
    Market,
    Limit
};

enum class Signal {
    Buy,
    Sell,
    Hold
};

inline std::string to_string(Side side) {
    return side == Side::Bid ? "BID" : "ASK";
}

inline std::string to_string(OrderType type) {
    return type == OrderType::Market ? "MARKET" : "LIMIT";
}

inline std::string to_string(Signal signal) {
    switch (signal) {
        case Signal::Buy:  return "BUY";
        case Signal::Sell: return "SELL";
        default:           return "HOLD";
    }
}

inline std::string trim(const std::string& s) {
    auto begin = s.begin();
    while (begin != s.end() && std::isspace(static_cast<unsigned char>(*begin))) {
        ++begin;
    }

    auto end = s.end();
    do {
        --end;
    } while (end >= begin && std::isspace(static_cast<unsigned char>(*end)));

    return (begin <= end) ? std::string(begin, end + 1) : std::string{};
}

inline Side parse_side(const std::string& raw) {
    std::string s = trim(raw);
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });

    if (s == "BID" || s == "BUY") {
        return Side::Bid;
    }
    if (s == "ASK" || s == "SELL") {
        return Side::Ask;
    }
    throw std::runtime_error("Invalid side: " + raw);
}