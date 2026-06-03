#include "MarketDataFeed.hpp"
#include "MatchingEngine.hpp"
#include "OrderBook.hpp"
#include "Portfolio.hpp"
#include "Types.hpp"

#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <sstream>
#include <variant>
#include <vector>

namespace {
bool near(double a, double b) {
    return std::fabs(a - b) < 1e-9;
}

void require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error("Test failed: " + message);
    }
}

std::filesystem::path write_temp_csv(const std::string& name, const std::string& content) {
    const auto path = std::filesystem::temp_directory_path() / name;
    std::ofstream file(path);
    file << content;
    return path;
}

void test_trim_handles_empty_strings() {
    require(trim("").empty(), "trim handles empty string");
    require(trim("   ").empty(), "trim handles whitespace-only string");
    require(trim("\t abc \n") == "abc", "trim removes surrounding whitespace");
}

void test_market_data_feed_parses_add_and_remove() {
    const auto path = write_temp_csv(
        "trading_feed_valid.csv",
        "timestamp,event_type,order_id,side,price,quantity\n"
        "1700000001,ADD,1001,BID,100.50,3\n"
        "1700000002,REMOVE,1001,,,\n"
    );

    MarketDataFeed feed(path.string());
    require(feed.has_next(), "feed has first event");

    const FeedEvent first = feed.next_event();
    const auto& add = std::get<OrderAdd>(first);
    require(add.timestamp == 1700000001, "ADD timestamp parsed");
    require(add.order_id == 1001, "ADD order_id parsed");
    require(add.side == Side::Bid, "ADD side parsed");
    require(near(add.price, 100.50), "ADD price parsed");
    require(add.quantity == 3, "ADD quantity parsed");

    require(feed.has_next(), "feed has second event");
    const FeedEvent second = feed.next_event();
    const auto& remove = std::get<OrderRemove>(second);
    require(remove.timestamp == 1700000002, "REMOVE timestamp parsed");
    require(remove.order_id == 1001, "REMOVE order_id parsed");

    std::filesystem::remove(path);
}

void test_market_data_feed_reports_bad_numbers() {
    const auto path = write_temp_csv(
        "trading_feed_invalid.csv",
        "timestamp,event_type,order_id,side,price,quantity\n"
        "1700000001,ADD,1001,ASK,not_a_price,3\n"
    );

    try {
        MarketDataFeed feed(path.string());
        static_cast<void>(feed.next_event());
        require(false, "invalid price should throw");
    } catch (const std::runtime_error& ex) {
        const std::string message = ex.what();
        require(
            message.find("Invalid price at CSV line 2") != std::string::npos,
            "invalid price reports line number"
        );
    }

    std::filesystem::remove(path);
}

void test_order_book_matches_crossing_feed_orders() {
    OrderBook book;
    book.on_add(OrderAdd{1, 1, Side::Ask, 100.0, 3});
    book.on_add(OrderAdd{2, 2, Side::Bid, 101.0, 5});

    require(book.asks().empty(), "crossing feed order removes ask");
    require(book.bids().size() == 1, "crossing feed order leaves one bid");
    require(book.bids().front().order_id == 2, "remaining bid keeps incoming id");
    require(book.bids().front().quantity == 2, "remaining bid has residual quantity");

    MarketData md = book.snapshot(2);
    require(near(md.last_price, 100.0), "feed crossing updates last price");
    require(md.volume == 3, "feed crossing updates last volume");

    book.on_add(OrderAdd{3, 3, Side::Ask, 102.0, 4});
    require(near(book.best_bid(), 101.0), "best bid after non-crossing ask");
    require(near(book.best_ask(), 102.0), "best ask after non-crossing ask");
    require(near(book.spread(), 1.0), "positive spread after feed matching");

    book.on_add(OrderAdd{4, 4, Side::Ask, 100.5, 1});
    require(book.bids().front().quantity == 1, "partial feed match reduces resting bid");
    require(near(book.best_ask(), 102.0), "fully matched ask is not added");

    md = book.snapshot(4);
    require(near(md.last_price, 101.0), "partial feed match updates last price");
    require(md.volume == 1, "partial feed match updates last volume");
}

void test_matching_engine_is_all_or_nothing() {
    OrderBook book;
    book.on_add(OrderAdd{1, 1, Side::Ask, 100.0, 1});
    book.on_add(OrderAdd{2, 2, Side::Ask, 101.0, 1});

    MatchingEngine engine(book);
    Order buy{10, 3, Side::Bid, OrderType::Limit, 100.5, 2, "test"};

    auto trades = engine.match(buy);
    require(trades.empty(), "unfillable AON limit order rejected");
    require(book.asks().size() == 2, "AON reject leaves asks untouched");
    require(book.asks()[0].quantity == 1, "first ask unchanged after AON reject");
    require(book.asks()[1].quantity == 1, "second ask unchanged after AON reject");

    buy.price = 101.0;
    trades = engine.match(buy);
    require(trades.size() == 2, "fillable AON order crosses two levels");
    const int total_qty = std::accumulate(
        trades.begin(),
        trades.end(),
        0,
        [](int sum, const Trade& trade) { return sum + trade.quantity; }
    );
    require(total_qty == 2, "fillable AON order fills full quantity");
    require(book.asks().empty(), "filled asks removed from book");

    const MarketData md = book.snapshot(3);
    require(near(md.last_price, 101.0), "strategy matching updates last price");
    require(md.volume == 2, "strategy matching updates last volume");
}

void test_remove_after_execution_is_silent() {
    OrderBook book;
    book.on_add(OrderAdd{1, 1, Side::Ask, 100.0, 1});
    book.on_add(OrderAdd{2, 2, Side::Bid, 101.0, 1});

    std::ostringstream captured;
    auto* original = std::cout.rdbuf(captured.rdbuf());
    book.on_remove(OrderRemove{3, 1});
    book.on_remove(OrderRemove{4, 2});
    std::cout.rdbuf(original);

    require(captured.str().empty(), "REMOVE after execution is silent");

    captured.str({});
    captured.clear();
    original = std::cout.rdbuf(captured.rdbuf());
    book.on_remove(OrderRemove{5, 999});
    std::cout.rdbuf(original);

    require(
        captured.str().find("unknown order_id=999") != std::string::npos,
        "REMOVE for unknown id still warns"
    );
}

void test_portfolio_risk_and_pnl() {
    Portfolio portfolio(1'000.0, 3);

    const Order buy3{1, 1, Side::Bid, OrderType::Market, 0.0, 3, "test"};
    const Order buy4{2, 2, Side::Bid, OrderType::Market, 0.0, 4, "test"};
    require(portfolio.pre_trade_check(buy3), "risk accepts max-sized position");
    require(!portfolio.pre_trade_check(buy4), "risk rejects oversized position");

    portfolio.on_trade(Trade{1, 1, 10, 1, Side::Bid, 100.0, 3, "test"});
    require(portfolio.position() == 3, "buy trade updates position");
    require(near(portfolio.cash(), 700.0), "buy trade updates cash");

    portfolio.on_trade(Trade{2, 2, 11, 2, Side::Ask, 110.0, 1, "test"});
    require(portfolio.position() == 2, "sell trade updates position");
    require(near(portfolio.cash(), 810.0), "sell trade updates cash");
    require(near(portfolio.realized_pnl(), 10.0), "sell trade realizes pnl");
    require(near(portfolio.equity(100.0), 1'010.0), "portfolio computes equity");
}
}

int main() {
    test_trim_handles_empty_strings();
    test_market_data_feed_parses_add_and_remove();
    test_market_data_feed_reports_bad_numbers();
    test_order_book_matches_crossing_feed_orders();
    test_matching_engine_is_all_or_nothing();
    test_remove_after_execution_is_silent();
    test_portfolio_risk_and_pnl();
    return 0;
}
