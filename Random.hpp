#pragma once
#include "default.hpp"

class Random {
   private:
    std::random_device rd;
    std::mt19937 gen;

   public:
    Random() : gen{rd()} {}

    Random(Random&&) = delete;
    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;

    inline int64_t getRand() {
        std::uniform_int_distribution<> rand;
        return rand(gen);
    }

    inline int64_t getRand(int maxN) {
        std::uniform_int_distribution<> rand(0, maxN);
        return rand(gen);
    }

    inline int64_t getRand(int minN, int maxN) {
        std::uniform_int_distribution<> rand(minN, maxN);
        return rand(gen);
    }
};