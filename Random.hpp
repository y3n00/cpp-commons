#pragma once
#include <random>

#ifdef RANDOM_TEMPLATES
template <typename T>
class Random {
   private:
    static std::mt19937 gen;

   public:
    Random() = default;
    Random(Random&&) = delete;
    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;

    [[nodiscard]] static T
    getRand() {
        return std::uniform_int_distribution()(gen);
    }

    [[nodiscard]] static T
    getRand(T maxN) {
        return std::uniform_int_distribution(uint64_t(0), maxN)(gen);
    }

    [[nodiscard]] static T
    getRand(T minN, T maxN) {
        return std::uniform_int_distribution(minN, maxN)(gen);
    }
};

template <typename T>
std::mt19937 Random<T>::gen{std::random_device{}()};
#else

class Random {
   private:
    static std::mt19937 gen;

   public:
    Random() = default;
    Random(Random&&) = delete;
    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;

    [[nodiscard]] static auto
    getRand() {
        return std::uniform_int_distribution()(gen);
    }

    [[nodiscard]] static auto
    getRand(uint64_t maxN) {
        return std::uniform_int_distribution(uint64_t(0), maxN)(gen);
    }

    [[nodiscard]] static auto
    getRand(int64_t minN, int64_t maxN) {
        return std::uniform_int_distribution(minN, maxN)(gen);
    }
};

std::mt19937 Random::gen{std::random_device{}()};
#endif