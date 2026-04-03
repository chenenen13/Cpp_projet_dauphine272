#include "Reporting.hpp"
#include "RiskMetrics.hpp"

#include <iostream>
#include <iomanip>

void Reporting::record_equity(std::int64_t timestamp, double equity_value) {
    if (!equity_curve_.empty() && equity_curve_.back().first == timestamp) {
        equity_curve_.back().second = equity_value;
    } else {
        equity_curve_.push_back({timestamp, equity_value});
    }
}

const std::vector<std::pair<std::int64_t, double>>& Reporting::equity_curve() const {
    return equity_curve_;
}

void Reporting::print_final_report(const Portfolio& portfolio, double final_mark) const {
    std::vector<double> eq;
    eq.reserve(equity_curve_.size());
    for (const auto& p : equity_curve_) {
        eq.push_back(p.second);
    }

    const auto returns = RiskMetrics::compute_returns(eq);
    const double vol = RiskMetrics::compute_annualized_volatility(returns);
    const double sharpe = RiskMetrics::compute_sharpe(returns);
    const double mdd = RiskMetrics::compute_max_drawdown(eq);
    const double win_rate = RiskMetrics::compute_win_rate(portfolio.pnl_increments());
    const double avg_win = RiskMetrics::compute_avg_win(portfolio.pnl_increments());
    const double avg_loss = RiskMetrics::compute_avg_loss(portfolio.pnl_increments());

    std::cout << "\n============================================================\n";
    std::cout << "                     FINAL REPORT                           \n";
    std::cout << "============================================================\n";
    std::cout << std::left
              << std::setw(28) << "Final mark price"        << ": " << std::fixed << std::setprecision(2) << final_mark << "\n"
              << std::setw(28) << "Final position"          << ": " << portfolio.position() << "\n"
              << std::setw(28) << "Cash"                    << ": " << std::fixed << std::setprecision(2) << portfolio.cash() << "\n"
              << std::setw(28) << "Realized P&L"            << ": " << std::fixed << std::setprecision(2) << portfolio.realized_pnl() << "\n"
              << std::setw(28) << "Total P&L"               << ": " << std::fixed << std::setprecision(2) << portfolio.total_pnl(final_mark) << "\n"
              << std::setw(28) << "Gross exposure"          << ": " << std::fixed << std::setprecision(2) << portfolio.gross_exposure(final_mark) << "\n"
              << std::setw(28) << "Net exposure"            << ": " << std::fixed << std::setprecision(2) << portfolio.net_exposure(final_mark) << "\n"
              << std::setw(28) << "Number of trades"        << ": " << portfolio.trade_count() << "\n"
              << std::setw(28) << "Sharpe Ratio annualized" << ": " << std::fixed << std::setprecision(4) << sharpe << "\n"
              << std::setw(28) << "Annualized volatility"   << ": " << std::fixed << std::setprecision(4) << vol << "\n"
              << std::setw(28) << "Max drawdown"            << ": " << std::fixed << std::setprecision(4) << mdd << "\n"
              << std::setw(28) << "Win rate"                << ": " << std::fixed << std::setprecision(4) << win_rate << "\n"
              << std::setw(28) << "Average win"             << ": " << std::fixed << std::setprecision(4) << avg_win << "\n"
              << std::setw(28) << "Average loss"            << ": " << std::fixed << std::setprecision(4) << avg_loss << "\n"
              << std::setw(28) << "Risk rejects"            << ": " << portfolio.rejected_risk_count() << "\n"
              << std::setw(28) << "Liquidity rejects"       << ": " << portfolio.rejected_liquidity_count() << "\n";
    std::cout << "============================================================\n";
}