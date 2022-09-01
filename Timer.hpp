#pragma once
#include "default.hpp"

class Timer {
    using ms = std::chrono::milliseconds;

   public:
    Timer(std::string_view title)
        : title{title},
          begin{std::chrono::high_resolution_clock::now()} {
        std::cerr << '[' << title << ']' << " is started\n";
    }

    ~Timer() {
        std::cerr << '[' << title << ']' << " Time took: " << std::chrono::duration_cast<ms>(std::chrono::high_resolution_clock::now() - begin).count() << "ms\n";
    }

   private:
    std::string_view title;
    std::chrono::time_point<std::chrono::high_resolution_clock> begin;
};
