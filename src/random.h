#pragma once

#include <chrono>
#include <random>

namespace Random {
    inline std::mt19937 generate() {
        std::random_device rd;

        std::seed_seq ss {static_cast<std::seed_seq::result_type>(std::chrono::steady_clock::now().time_since_epoch().count()),rd(), rd(), rd(), rd(), rd(), rd(), rd()};

        return std::mt19937(ss);
    }

    inline std::mt19937 mt{generate()};

    inline int randInt(const int min, const int max) {
        return std::uniform_int_distribution<int>{min, max}(mt);
    }

    template <typename T>
    T RandInt(T min, T max)
    {
        return std::uniform_int_distribution<T>{min, max}(mt);
    }

    template <typename R, typename S, typename T>
    R randInt(S min, T max)
    {
        return get<R>(static_cast<R>(min), static_cast<R>(max));
    }
}