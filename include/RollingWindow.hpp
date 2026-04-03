#pragma once

#include <deque>
#include <numeric>
#include <cmath>
#include <stdexcept>

template <typename T>
class RollingWindow {
public:
    explicit RollingWindow(std::size_t max_size)
        : max_size_(max_size) {
        if (max_size_ == 0) {
            throw std::runtime_error("RollingWindow size must be > 0");
        }
    }

    void push(const T& value) {
        if (data_.size() == max_size_) {
            data_.pop_front();
        }
        data_.push_back(value);
    }

    bool full() const {
        return data_.size() == max_size_;
    }

    std::size_t size() const {
        return data_.size();
    }

    bool empty() const {
        return data_.empty();
    }

    T mean() const {
        if (data_.empty()) {
            throw std::runtime_error("Cannot compute mean on empty RollingWindow");
        }
        T sum = std::accumulate(data_.begin(), data_.end(), static_cast<T>(0));
        return sum / static_cast<T>(data_.size());
    }

    T stddev() const {
        if (data_.size() < 2) {
            return static_cast<T>(0);
        }

        T m = mean();
        T acc = static_cast<T>(0);
        for (const auto& x : data_) {
            T d = x - m;
            acc += d * d;
        }
        return std::sqrt(acc / static_cast<T>(data_.size() - 1));
    }

    const std::deque<T>& values() const {
        return data_;
    }

private:
    std::size_t max_size_;
    std::deque<T> data_;
};