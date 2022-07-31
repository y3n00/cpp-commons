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

    int64_t operator()() {
        std::uniform_int_distribution rand;
        return rand(gen);
    }

    uint64_t operator()(uint64_t maxN) {
        std::uniform_int_distribution rand(uint64_t(0), maxN);
        return rand(gen);
    }

    int64_t operator()(int64_t minN, int64_t maxN) {
        std::uniform_int_distribution rand(minN, maxN);
        return rand(gen);
    }
};