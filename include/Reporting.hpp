#pragma once

#include "Portfolio.hpp"
#include <vector>
#include <utility>

class Reporting {
public:
    void record_equity(std::int64_t timestamp, double equity_value);
    double sharpe_annualized(double risk_free_rate = 0.0) const;
    void print_final_report(const Portfolio& portfolio, double final_mark) const;

private:
    std::vector<std::pair<std::int64_t, double>> equity_curve_;
};