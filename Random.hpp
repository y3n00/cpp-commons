#pragma once
#include <random>

class Random {
   private:
    static std::mt19937 gen;

   public:
    Random() = default;
    Random(Random&&) = delete;
    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;

    static int64_t getRand() {
        return std::uniform_int_distribution()(gen);
    }

    static int64_t getRand(uint64_t maxN) {
        return std::uniform_int_distribution(uint64_t(0), maxN)(gen);
    }

    static int64_t getRand(int64_t minN, int64_t maxN) {
        return std::uniform_int_distribution(minN, maxN)(gen);
    }
};

std::mt19937 Random::gen{std::random_device{}()};