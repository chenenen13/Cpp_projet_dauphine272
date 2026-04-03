cmake_minimum_required(VERSION 3.15)
project(TradingPlatform CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(trading_sim
    src/main.cpp
    src/MarketDataFeed.cpp
    src/OrderBook.cpp
    src/MatchingEngine.cpp
    src/Strategy.cpp
    src/StrategyEngine.cpp
    src/Portfolio.cpp
    src/Reporting.cpp
    src/RiskMetrics.cpp
    src/CsvWriter.cpp
    src/BacktestLogger.cpp
)

target_include_directories(trading_sim PRIVATE include)
target_compile_features(trading_sim PRIVATE cxx_std_17)

if (MSVC)
    target_compile_options(trading_sim PRIVATE /W4)
else()
    target_compile_options(trading_sim PRIVATE -Wall -Wextra -Wpedantic)
endif()
