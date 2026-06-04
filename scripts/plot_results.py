import argparse
from pathlib import Path

import matplotlib

matplotlib.use("Agg")

import matplotlib.pyplot as plt
import pandas as pd


STEP_DRAWSTYLE = "steps-post"


def read_csv(input_dir: Path, name: str) -> pd.DataFrame:
    path = input_dir / name
    if not path.exists():
        raise FileNotFoundError(f"Missing required file: {path}")
    return pd.read_csv(path)


def save_plot(output_dir: Path, file_name: str, show: bool) -> None:
    output_dir.mkdir(parents=True, exist_ok=True)
    plt.tight_layout()
    plt.savefig(output_dir / file_name, dpi=140)
    if show:
        plt.show()
    plt.close()


def plot_equity_curve(equity: pd.DataFrame, output_dir: Path, show: bool) -> None:
    plt.figure(figsize=(10, 5))
    plt.plot(equity["timestamp"], equity["equity"], drawstyle=STEP_DRAWSTYLE)
    plt.title("Equity Curve")
    plt.xlabel("Timestamp")
    plt.ylabel("Equity")
    plt.grid(True)
    save_plot(output_dir, "equity_curve.png", show)


def plot_pnl(equity: pd.DataFrame, output_dir: Path, show: bool) -> None:
    plt.figure(figsize=(10, 5))
    plt.plot(equity["timestamp"], equity["pnl"], drawstyle=STEP_DRAWSTYLE)
    plt.title("Cumulative PnL")
    plt.xlabel("Timestamp")
    plt.ylabel("PnL")
    plt.grid(True)
    save_plot(output_dir, "pnl.png", show)


def plot_market_and_trades(market: pd.DataFrame,
                           trades: pd.DataFrame,
                           output_dir: Path,
                           show: bool) -> None:
    plotted_market = market.copy()
    price_columns = ["best_bid", "best_ask", "last_price"]
    plotted_market[price_columns] = plotted_market[price_columns].mask(
        plotted_market[price_columns] <= 0
    )

    plt.figure(figsize=(10, 5))
    plt.plot(plotted_market["timestamp"], plotted_market["best_bid"],
             drawstyle=STEP_DRAWSTYLE, label="Best Bid")
    plt.plot(plotted_market["timestamp"], plotted_market["best_ask"],
             drawstyle=STEP_DRAWSTYLE, label="Best Ask")
    plt.plot(plotted_market["timestamp"], plotted_market["last_price"],
             drawstyle=STEP_DRAWSTYLE, label="Last Price")

    if not trades.empty:
        plt.scatter(trades["timestamp"], trades["price"], marker="x", label="Trades")

    plt.title("Market Data and Trades")
    plt.xlabel("Timestamp")
    plt.ylabel("Price")
    plt.legend()
    plt.grid(True)
    save_plot(output_dir, "market_and_trades.png", show)


def plot_position(equity: pd.DataFrame, output_dir: Path, show: bool) -> None:
    plt.figure(figsize=(10, 5))
    plt.plot(equity["timestamp"], equity["position"], drawstyle=STEP_DRAWSTYLE)
    plt.title("Position Over Time")
    plt.xlabel("Timestamp")
    plt.ylabel("Position")
    plt.grid(True)
    save_plot(output_dir, "position.png", show)


def main() -> None:
    parser = argparse.ArgumentParser(description="Plot trading simulation CSV outputs.")
    parser.add_argument(
        "--input",
        default="output",
        help="Directory containing market_data.csv, trades.csv and equity_curve.csv.",
    )
    parser.add_argument(
        "--out",
        default=None,
        help="Directory where PNG plots are written. Defaults to <input>/plots.",
    )
    parser.add_argument(
        "--show",
        action="store_true",
        help="Also display figures interactively after saving them.",
    )
    args = parser.parse_args()

    input_dir = Path(args.input)
    output_dir = Path(args.out) if args.out else input_dir / "plots"

    equity = read_csv(input_dir, "equity_curve.csv")
    trades = read_csv(input_dir, "trades.csv")
    market = read_csv(input_dir, "market_data.csv")

    plot_equity_curve(equity, output_dir, args.show)
    plot_pnl(equity, output_dir, args.show)
    plot_market_and_trades(market, trades, output_dir, args.show)
    plot_position(equity, output_dir, args.show)

    print(f"Plots written to {output_dir}")


if __name__ == "__main__":
    main()
