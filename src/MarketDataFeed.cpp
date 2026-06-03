#include "MarketDataFeed.hpp"
#include "Types.hpp"

#include <algorithm>
#include <cctype>
#include <exception>
#include <sstream>
#include <stdexcept>
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

std::string uppercase(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });
    return value;
}

void validate_header(const std::string& header) {
    const auto fields = split_csv_line(header);
    const std::vector<std::string> expected = {
        "timestamp", "event_type", "order_id", "side", "price", "quantity"
    };

    if (fields != expected) {
        throw std::runtime_error(
            "Invalid CSV header: expected timestamp,event_type,order_id,side,price,quantity"
        );
    }
}

std::runtime_error parse_error(std::size_t line_number,
                               const std::string& field_name,
                               const std::string& raw) {
    return std::runtime_error(
        "Invalid " + field_name + " at CSV line " + std::to_string(line_number) +
        ": '" + raw + "'"
    );
}

template <typename Parser>
auto parse_required_number(const std::string& raw,
                           std::size_t line_number,
                           const std::string& field_name,
                           Parser parser) {
    if (raw.empty()) {
        throw parse_error(line_number, field_name, raw);
    }

    try {
        std::size_t pos = 0;
        auto value = parser(raw, pos);
        if (pos != raw.size()) {
            throw parse_error(line_number, field_name, raw);
        }
        return value;
    } catch (const std::runtime_error&) {
        throw;
    } catch (const std::exception&) {
        throw parse_error(line_number, field_name, raw);
    }
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
    validate_header(trim(header));
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

    const auto timestamp = parse_required_number(
        fields[0],
        line_number_,
        "timestamp",
        [](const std::string& raw, std::size_t& pos) {
            return std::stoll(raw, &pos);
        }
    );
    const std::string event_type = uppercase(fields[1]);

    if (event_type == "ADD") {
        if (fields[2].empty() || fields[3].empty() || fields[4].empty() || fields[5].empty()) {
            throw std::runtime_error("Malformed ADD line at CSV line " + std::to_string(line_number_));
        }

        OrderAdd ev;
        ev.timestamp = timestamp;
        ev.order_id = parse_required_number(
            fields[2],
            line_number_,
            "order_id",
            [](const std::string& raw, std::size_t& pos) {
                return std::stoull(raw, &pos);
            }
        );
        ev.side = parse_side(fields[3]);
        ev.price = parse_required_number(
            fields[4],
            line_number_,
            "price",
            [](const std::string& raw, std::size_t& pos) {
                return std::stod(raw, &pos);
            }
        );
        ev.quantity = parse_required_number(
            fields[5],
            line_number_,
            "quantity",
            [](const std::string& raw, std::size_t& pos) {
                return std::stoi(raw, &pos);
            }
        );

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
        ev.order_id = parse_required_number(
            fields[2],
            line_number_,
            "order_id",
            [](const std::string& raw, std::size_t& pos) {
                return std::stoull(raw, &pos);
            }
        );
        return ev;
    }

    throw std::runtime_error(
        "Unknown event_type at CSV line " + std::to_string(line_number_) + ": " + fields[1]
    );
}
