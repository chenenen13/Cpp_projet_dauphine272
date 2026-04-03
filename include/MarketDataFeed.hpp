#pragma once

#include "Events.hpp"
#include <fstream>
#include <string>

class MarketDataFeed {
public:
    explicit MarketDataFeed(const std::string& csv_path);

    bool has_next();
    FeedEvent next_event();

private:
    std::ifstream file_;
    std::size_t line_number_{0};

    FeedEvent parse_line(const std::string& line) const;
};