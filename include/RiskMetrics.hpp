#pragma once

#include <vector>

class RiskMetrics {
public:
    static std::vector<double> compute_returns(const std::vector<double>& equity_curve);
    static double compute_annualized_volatility(const std::vector<double>& returns);
    static double compute_sharpe(const std::vector<double>& returns, double risk_free_rate = 0.0);
    static double compute_max_drawdown(const std::vector<double>& equity_curve);
    static double compute_win_rate(const std::vector<double>& pnl_increments);
    static double compute_avg_win(const std::vector<double>& pnl_increments);
    static double compute_avg_loss(const std::vector<double>& pnl_increments);
};
