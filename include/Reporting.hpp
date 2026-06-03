#pragma once

#include "Portfolio.hpp"
#include <vector>
#include <utility>
#include <string>

class Reporting {
public:
    void record_equity(std::int64_t timestamp, double equity_value);
    void print_final_report(const Portfolio& portfolio, double final_mark) const;
    void write_risk_report(const std::string& file_path,
                           const Portfolio& portfolio,
                           double final_mark) const;

private:
    std::vector<std::pair<std::int64_t, double>> equity_curve_;
};
