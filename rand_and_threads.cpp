#include <sstream>

#include "default.hpp"

void foo(const char* title, size_t maxValue) {
    Timer timer(title);
    Random rand;
    size_t counter{};
    std::stringstream sstr;
    for (size_t k{}; k <= 15; ++k) {
        auto randValue{rand.getRand(maxValue)};
        sstr << (counter += randValue) << ' ';
        std::this_thread::sleep_for(std::chrono::milliseconds(randValue));
    }
    std::cerr << sstr.str() << '\n';
}

int main() {
    Timer timer("MAIN");
    std::thread t1(foo, "T1", 50);
    std::thread t2(foo, "T2", 100);
    std::thread t3(foo, "T3", 500);

    t1.join();
    t2.join();
    t3.join();
}

/* POSSIBLE OUTPUT
[MAIN]
[T1]
[T2]
[T3]
22 61 71 98 132 136 165 209 218 258 285 315 329 371 404 407
[T1] Time took: 430ms
67 167 180 267 276 372 389 468 497 563 625 651 674 747 814 826
[T2] Time took: 859ms
97 103 327 566 851 1006 1265 1266 1752 2176 2433 2693 2767 3003 3503 3540
[T3] Time took: 3571ms
[MAIN] Time took: 3571ms
*/