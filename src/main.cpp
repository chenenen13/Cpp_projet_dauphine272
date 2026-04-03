#include "MarketDataFeed.hpp"
#include "OrderBook.hpp"
#include "MatchingEngine.hpp"
#include "StrategyEngine.hpp"
#include "Portfolio.hpp"
#include "Reporting.hpp"

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    try {
        const std::string csv_path = (argc >= 2) ? argv[1] : "data/test_events.csv";
        const std::string strategy_name = (argc >= 3) ? argv[2] : "momentum";

        std::cout << "Loading CSV: " << csv_path << "\n";
        std::cout << "Using strategy: " << strategy_name << "\n";

        MarketDataFeed feed(csv_path);
        OrderBook order_book;
        MatchingEngine matching_engine(order_book);
        StrategyEngine strategy_engine(make_strategy(strategy_name));
        Portfolio portfolio(1'000'000.0, 10);
        Reporting reporting;

        double latest_mark = 0.0;

        while (feed.has_next()) {
            FeedEvent event = feed.next_event();

            std::visit([&](const auto& ev) {
                using T = std::decay_t<decltype(ev)>;

                if constexpr (std::is_same_v<T, OrderAdd>) {
                    order_book.on_add(ev);
                    std::cout << "[FEED][ADD] ts=" << ev.timestamp
                              << " id=" << ev.order_id
                              << " side=" << to_string(ev.side)
                              << " px=" << ev.price
                              << " qty=" << ev.quantity << "\n";

                    MarketData md = order_book.snapshot(ev.timestamp);
                    latest_mark = md.last_price;
                    reporting.record_equity(md.timestamp, portfolio.equity(latest_mark));

                    auto maybe_order = strategy_engine.maybe_create_order(md, ev.timestamp);
                    if (maybe_order) {
                        const auto& order = *maybe_order;

                        std::cout << "[STRATEGY][" << order.strategy_name << "] "
                                  << to_string(order.side)
                                  << " " << to_string(order.type)
                                  << " qty=" << order.quantity;

                        if (order.type == OrderType::Limit) {
                            std::cout << " px=" << order.price;
                        }
                        std::cout << "\n";

                        if (!portfolio.pre_trade_check(order)) {
                            portfolio.add_risk_reject();
                            std::cout << "[RISK] Order rejected: pre-trade max position exceeded\n";
                        } else {
                            auto trades = matching_engine.match(order);

                            if (trades.empty()) {
                                portfolio.add_liquidity_reject();
                                std::cout << "[MATCHING] Order rejected: insufficient liquidity (all-or-nothing)\n";
                            } else {
                                for (const auto& tr : trades) {
                                    portfolio.on_trade(tr);
                                    std::cout << "[TRADE] id=" << tr.trade_id
                                              << " resting_id=" << tr.resting_order_id
                                              << " aggr_id=" << tr.aggressor_order_id
                                              << " px=" << tr.price
                                              << " qty=" << tr.quantity
                                              << " side=" << to_string(tr.aggressor_side)
                                              << "\n";
                                }

                                MarketData post_trade_md = order_book.snapshot(ev.timestamp);
                                latest_mark = post_trade_md.last_price;
                                reporting.record_equity(post_trade_md.timestamp, portfolio.equity(latest_mark));
                            }
                        }
                    }
                } else if constexpr (std::is_same_v<T, OrderRemove>) {
                    order_book.on_remove(ev);
                    std::cout << "[FEED][REMOVE] ts=" << ev.timestamp
                              << " id=" << ev.order_id << "\n";

                    MarketData md = order_book.snapshot(ev.timestamp);
                    latest_mark = md.last_price;
                    reporting.record_equity(md.timestamp, portfolio.equity(latest_mark));
                }
            }, event);
        }

        if (latest_mark <= 0.0) {
            latest_mark = 0.0;
        }

        order_book.display(5);
        reporting.print_final_report(portfolio, latest_mark);

        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "\n[FATAL ERROR] " << ex.what() << "\n";
        return 1;
    }
}