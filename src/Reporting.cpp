#include "Reporting.hpp"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <numeric>

void Reporting::record_equity(std::int64_t timestamp, double equity_value) {
    if (!equity_curve_.empty() && equity_curve_.back().first == timestamp) {
        equity_curve_.back().second = equity_value;
    } else {
        equity_curve_.push_back({timestamp, equity_value});
    }
}

double Reporting::sharpe_annualized(double risk_free_rate) const {
    if (equity_curve_.size() < 2) {
        return 0.0;
    }

    std::vector<double> returns;
    returns.reserve(equity_curve_.size() - 1);

    for (std::size_t i = 1; i < equity_curve_.size(); ++i) {
        const double prev = equity_curve_[i - 1].second;
        const double curr = equity_curve_[i].second;
        if (prev > 0.0) {
            returns.push_back(curr / prev - 1.0);
        }
    }

    if (returns.size() < 2) {
        return 0.0;
    }

    const double mean = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();

    double variance = 0.0;
    for (double r : returns) {
        const double d = r - mean;
        variance += d * d;
    }
    variance /= static_cast<double>(returns.size() - 1);

    const double vol = std::sqrt(variance);
    if (vol <= 1e-12) {
        return 0.0;
    }

    return ((mean - risk_free_rate) / vol) * std::sqrt(252.0);
}

void Reporting::print_final_report(const Portfolio& portfolio, double final_mark) const {
    std::cout << "\n============================================================\n";
    std::cout << "                     FINAL REPORT                           \n";
    std::cout << "============================================================\n";
    std::cout << std::left
              << std::setw(28) << "Final mark price"        << ": " << std::fixed << std::setprecision(2) << final_mark << "\n"
              << std::setw(28) << "Final position"          << ": " << portfolio.position() << "\n"
              << std::setw(28) << "Cash"                    << ": " << std::fixed << std::setprecision(2) << portfolio.cash() << "\n"
              << std::setw(28) << "Realized P&L"            << ": " << std::fixed << std::setprecision(2) << portfolio.realized_pnl() << "\n"
              << std::setw(28) << "Total P&L"               << ": " << std::fixed << std::setprecision(2) << portfolio.total_pnl(final_mark) << "\n"
              << std::setw(28) << "Number of trades"        << ": " << portfolio.trade_count() << "\n"
              << std::setw(28) << "Sharpe Ratio annualized" << ": " << std::fixed << std::setprecision(4) << sharpe_annualized(0.0) << "\n"
              << std::setw(28) << "Risk rejects"            << ": " << portfolio.rejected_risk_count() << "\n"
              << std::setw(28) << "Liquidity rejects"       << ": " << portfolio.rejected_liquidity_count() << "\n";
    std::cout << "============================================================\n";
}