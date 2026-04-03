#pragma once

#include "Events.hpp"
#include "RollingWindow.hpp"
#include <memory>
#include <string>

class Strategy {
public:
    virtual ~Strategy() = default;

    virtual Signal on_market_data(const MarketData& data) = 0;
    virtual std::string name() const = 0;
    virtual OrderType preferred_order_type() const = 0;
    virtual int quantity() const = 0;
};

class MomentumStrategy : public Strategy {
public:
    MomentumStrategy(std::size_t fast_window,
                     std::size_t slow_window,
                     double epsilon,
                     int qty,
                     OrderType type);

    Signal on_market_data(const MarketData& data) override;
    std::string name() const override;
    OrderType preferred_order_type() const override;
    int quantity() const override;

private:
    RollingWindow<double> fast_;
    RollingWindow<double> slow_;
    double epsilon_;
    int qty_;
    OrderType type_;
};

class MeanReversionStrategy : public Strategy {
public:
    MeanReversionStrategy(std::size_t window,
                          double z_threshold,
                          int qty,
                          OrderType type);

    Signal on_market_data(const MarketData& data) override;
    std::string name() const override;
    OrderType preferred_order_type() const override;
    int quantity() const override;

private:
    RollingWindow<double> prices_;
    double z_threshold_;
    int qty_;
    OrderType type_;
};

std::unique_ptr<Strategy> make_strategy(const std::string& strategy_name);

class BollingerBandsStrategy : public Strategy {
public:
    BollingerBandsStrategy(std::size_t window,
                           double num_stddev,
                           int qty,
                           OrderType type);

    Signal on_market_data(const MarketData& data) override;
    std::string name() const override;
    OrderType preferred_order_type() const override;
    int quantity() const override;

private:
    RollingWindow<double> prices_;
    double num_stddev_;
    int qty_;
    OrderType type_;
};

class MovingAverageCrossStrategy : public Strategy {
public:
    MovingAverageCrossStrategy(std::size_t fast_window,
                               std::size_t slow_window,
                               int qty,
                               OrderType type);

    Signal on_market_data(const MarketData& data) override;
    std::string name() const override;
    OrderType preferred_order_type() const override;
    int quantity() const override;

private:
    RollingWindow<double> fast_;
    RollingWindow<double> slow_;
    int qty_;
    OrderType type_;
};