#include "Reporting.hpp"
#include "CsvWriter.hpp"
#include "RiskMetrics.hpp"

#include <iostream>
#include <iomanip>
#include <sstream>

namespace {
struct ReportMetrics {
    double annualized_volatility{};
    double sharpe{};
    double max_drawdown{};
    double win_rate{};
    double avg_win{};
    double avg_loss{};
};

std::vector<double> collect_equity_values(const std::vector<std::pair<std::int64_t, double>>& equity_curve) {
    std::vector<double> eq;
    eq.reserve(equity_curve.size());
    for (const auto& p : equity_curve) {
        eq.push_back(p.second);
    }
    return eq;
}

ReportMetrics compute_metrics(const std::vector<std::pair<std::int64_t, double>>& equity_curve,
                              const Portfolio& portfolio) {
    const auto eq = collect_equity_values(equity_curve);
    const auto returns = RiskMetrics::compute_returns(eq);

    ReportMetrics metrics;
    metrics.annualized_volatility = RiskMetrics::compute_annualized_volatility(returns);
    metrics.sharpe = RiskMetrics::compute_sharpe(returns);
    metrics.max_drawdown = RiskMetrics::compute_max_drawdown(eq);
    metrics.win_rate = RiskMetrics::compute_win_rate(portfolio.pnl_increments());
    metrics.avg_win = RiskMetrics::compute_avg_win(portfolio.pnl_increments());
    metrics.avg_loss = RiskMetrics::compute_avg_loss(portfolio.pnl_increments());
    return metrics;
}

std::string format_double(double value, int precision = 10) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}
}

void Reporting::record_equity(std::int64_t timestamp, double equity_value) {
    if (!equity_curve_.empty() && equity_curve_.back().first == timestamp) {
        equity_curve_.back().second = equity_value;
    } else {
        equity_curve_.push_back({timestamp, equity_value});
    }
}

void Reporting::print_final_report(const Portfolio& portfolio, double final_mark) const {
    const ReportMetrics metrics = compute_metrics(equity_curve_, portfolio);

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
              << std::setw(28) << "Sharpe Ratio annualized" << ": " << std::fixed << std::setprecision(4) << metrics.sharpe << "\n"
              << std::setw(28) << "Annualized volatility"   << ": " << std::fixed << std::setprecision(8) << metrics.annualized_volatility << "\n"
              << std::setw(28) << "Max drawdown"            << ": " << std::fixed << std::setprecision(8) << metrics.max_drawdown << "\n"
              << std::setw(28) << "Win rate"                << ": " << std::fixed << std::setprecision(4) << metrics.win_rate << "\n"
              << std::setw(28) << "Average win"             << ": " << std::fixed << std::setprecision(4) << metrics.avg_win << "\n"
              << std::setw(28) << "Average loss"            << ": " << std::fixed << std::setprecision(4) << metrics.avg_loss << "\n"
              << std::setw(28) << "Risk rejects"            << ": " << portfolio.rejected_risk_count() << "\n"
              << std::setw(28) << "Liquidity rejects"       << ": " << portfolio.rejected_liquidity_count() << "\n";
    std::cout << "============================================================\n";
}

void Reporting::write_risk_report(const std::string& file_path,
                                  const Portfolio& portfolio,
                                  double final_mark) const {
    const ReportMetrics metrics = compute_metrics(equity_curve_, portfolio);
    CsvWriter writer(file_path);

    writer.write_row({"metric", "value"});
    writer.write_row({"final_mark_price", format_double(final_mark, 2)});
    writer.write_row({"final_position", std::to_string(portfolio.position())});
    writer.write_row({"cash", format_double(portfolio.cash(), 2)});
    writer.write_row({"realized_pnl", format_double(portfolio.realized_pnl(), 2)});
    writer.write_row({"total_pnl", format_double(portfolio.total_pnl(final_mark), 2)});
    writer.write_row({"gross_exposure", format_double(portfolio.gross_exposure(final_mark), 2)});
    writer.write_row({"net_exposure", format_double(portfolio.net_exposure(final_mark), 2)});
    writer.write_row({"number_of_trades", std::to_string(portfolio.trade_count())});
    writer.write_row({"sharpe_ratio_annualized", format_double(metrics.sharpe, 8)});
    writer.write_row({"annualized_volatility", format_double(metrics.annualized_volatility, 10)});
    writer.write_row({"max_drawdown", format_double(metrics.max_drawdown, 10)});
    writer.write_row({"win_rate", format_double(metrics.win_rate, 8)});
    writer.write_row({"average_win", format_double(metrics.avg_win, 8)});
    writer.write_row({"average_loss", format_double(metrics.avg_loss, 8)});
    writer.write_row({"risk_rejects", std::to_string(portfolio.rejected_risk_count())});
    writer.write_row({"liquidity_rejects", std::to_string(portfolio.rejected_liquidity_count())});
}
