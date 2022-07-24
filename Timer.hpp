#pragma once
#include "default.hpp"

class Timer {
   private:
    using ms = std::chrono::milliseconds;
    std::chrono::time_point<std::chrono::high_resolution_clock> begin;
    const char* title;

   public:
    Timer(const char* title)
        : title{title},
          begin{std::chrono::high_resolution_clock::now()} {
        std::cerr << '[' << title << ']' << '\n';
    }

    ~Timer() {
        std::cerr << '[' << title << ']' << " Time took: " << std::chrono::duration_cast<ms>(std::chrono::high_resolution_clock::now() - begin).count() << "ms\n";
    }
};
