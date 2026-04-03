#include "RiskMetrics.hpp"

#include <cmath>
#include <numeric>
#include <algorithm>

std::vector<double> RiskMetrics::compute_returns(const std::vector<double>& equity_curve) {
    std::vector<double> rets;
    if (equity_curve.size() < 2) {
        return rets;
    }

    for (std::size_t i = 1; i < equity_curve.size(); ++i) {
        if (equity_curve[i - 1] > 0.0) {
            rets.push_back(equity_curve[i] / equity_curve[i - 1] - 1.0);
        }
    }
    return rets;
}

double RiskMetrics::compute_annualized_volatility(const std::vector<double>& returns) {
    if (returns.size() < 2) {
        return 0.0;
    }

    const double mean = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    double var = 0.0;
    for (double r : returns) {
        const double d = r - mean;
        var += d * d;
    }
    var /= static_cast<double>(returns.size() - 1);
    return std::sqrt(var) * std::sqrt(252.0);
}

double RiskMetrics::compute_sharpe(const std::vector<double>& returns, double risk_free_rate) {
    if (returns.size() < 2) {
        return 0.0;
    }

    const double mean = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    double var = 0.0;
    for (double r : returns) {
        const double d = r - mean;
        var += d * d;
    }
    var /= static_cast<double>(returns.size() - 1);

    const double vol = std::sqrt(var);
    if (vol <= 1e-12) {
        return 0.0;
    }

    return ((mean - risk_free_rate) / vol) * std::sqrt(252.0);
}

double RiskMetrics::compute_max_drawdown(const std::vector<double>& equity_curve) {
    if (equity_curve.empty()) {
        return 0.0;
    }

    double peak = equity_curve.front();
    double max_dd = 0.0;

    for (double x : equity_curve) {
        peak = std::max(peak, x);
        if (peak > 0.0) {
            const double dd = (peak - x) / peak;
            max_dd = std::max(max_dd, dd);
        }
    }

    return max_dd;
}

double RiskMetrics::compute_win_rate(const std::vector<double>& pnl_increments) {
    if (pnl_increments.empty()) {
        return 0.0;
    }

    int wins = 0;
    int count = 0;
    for (double x : pnl_increments) {
        if (x != 0.0) {
            ++count;
            if (x > 0.0) {
                ++wins;
            }
        }
    }

    if (count == 0) {
        return 0.0;
    }
    return static_cast<double>(wins) / count;
}

double RiskMetrics::compute_avg_win(const std::vector<double>& pnl_increments) {
    double sum = 0.0;
    int n = 0;
    for (double x : pnl_increments) {
        if (x > 0.0) {
            sum += x;
            ++n;
        }
    }
    return n > 0 ? sum / n : 0.0;
}

double RiskMetrics::compute_avg_loss(const std::vector<double>& pnl_increments) {
    double sum = 0.0;
    int n = 0;
    for (double x : pnl_increments) {
        if (x < 0.0) {
            sum += x;
            ++n;
        }
    }
    return n > 0 ? sum / n : 0.0;
}