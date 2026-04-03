#include "MarketDataFeed.hpp"
#include "Types.hpp"

#include <sstream>
#include <vector>

namespace {
std::vector<std::string> split_csv_line(const std::string& line) {
    std::vector<std::string> out;
    std::stringstream ss(line);
    std::string item;

    while (std::getline(ss, item, ',')) {
        out.push_back(trim(item));
    }

    if (!line.empty() && line.back() == ',') {
        out.push_back({});
    }

    return out;
}
}

MarketDataFeed::MarketDataFeed(const std::string& csv_path)
    : file_(csv_path) {
    if (!file_.is_open()) {
        throw std::runtime_error("Could not open CSV file: " + csv_path);
    }

    std::string header;
    if (!std::getline(file_, header)) {
        throw std::runtime_error("CSV file is empty: " + csv_path);
    }
    line_number_ = 1;
}

bool MarketDataFeed::has_next() {
    return static_cast<bool>(file_.peek() != EOF);
}

FeedEvent MarketDataFeed::next_event() {
    std::string line;
    if (!std::getline(file_, line)) {
        throw std::runtime_error("No more events to read from CSV");
    }

    ++line_number_;

    if (trim(line).empty()) {
        throw std::runtime_error("Empty line at CSV line " + std::to_string(line_number_));
    }

    return parse_line(line);
}

FeedEvent MarketDataFeed::parse_line(const std::string& line) const {
    auto fields = split_csv_line(line);

    if (fields.size() != 6) {
        throw std::runtime_error(
            "Malformed CSV line " + std::to_string(line_number_) +
            ": expected 6 columns, got " + std::to_string(fields.size())
        );
    }

    const std::int64_t timestamp = std::stoll(fields[0]);
    std::string event_type = fields[1];
    std::transform(event_type.begin(), event_type.end(), event_type.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });

    if (event_type == "ADD") {
        if (fields[2].empty() || fields[3].empty() || fields[4].empty() || fields[5].empty()) {
            throw std::runtime_error("Malformed ADD line at CSV line " + std::to_string(line_number_));
        }

        OrderAdd ev;
        ev.timestamp = timestamp;
        ev.order_id = std::stoull(fields[2]);
        ev.side = parse_side(fields[3]);
        ev.price = std::stod(fields[4]);
        ev.quantity = std::stoi(fields[5]);

        if (ev.price <= 0.0 || ev.quantity <= 0) {
            throw std::runtime_error("Invalid ADD values at CSV line " + std::to_string(line_number_));
        }

        return ev;
    }

    if (event_type == "REMOVE") {
        if (fields[2].empty()) {
            throw std::runtime_error("Malformed REMOVE line at CSV line " + std::to_string(line_number_));
        }

        OrderRemove ev;
        ev.timestamp = timestamp;
        ev.order_id = std::stoull(fields[2]);
        return ev;
    }

    throw std::runtime_error(
        "Unknown event_type at CSV line " + std::to_string(line_number_) + ": " + fields[1]
    );
}
