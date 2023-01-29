#pragma once
#include <random>

template <typename T = int64_t>
class Random {
   private:
    static std::mt19937 gen;

   public:
    Random() = default;
    Random(Random&&) = delete;
    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;

    [[nodiscard]] static T
    get() {
        return std::uniform_int_distribution()(gen);
    }

    [[nodiscard]] static T
    get(T maxN) {
        return std::uniform_int_distribution(T{}, maxN)(gen);
    }

    [[nodiscard]] static T
    get(T minN, T maxN) {
        return std::uniform_int_distribution(minN, maxN)(gen);
    }
};

template <typename T>
std::mt19937 Random<T>::gen{std::random_device{}()};