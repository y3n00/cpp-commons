#include <sstream>

#include "default.hpp"

void foo(const std::string& title, size_t maxValue) {
    Timer::SimpleTimer<Timer::ms> timer(title);

    std::stringstream sstr;
    for (size_t k{}; k <= 15; ++k) {
        auto randValue{Random::getRand(maxValue)};
        sstr << randValue << ' ';
        std::this_thread::sleep_for(Timer::ms(randValue));
    }
    PRINT(sstr.str());
}

int main() {
    Timer::SimpleTimer<Timer::ms> timer("MAIN");

    std::thread t1(foo, "T1", 50);
    std::thread t2(foo, "T2", 100);
    std::thread t3(foo, "T3", 500);

    t1.join();
    t2.join();
    t3.join();
}

/* POSSIBLE OUTPUT
[MAIN] is started
[T1] is started
[T3] is started
[T2] is started
36 1 5 40 45 38 45 48 23 50 34 11 44 47 31 11
[T1] Time took: 540ms
91 89 12 77 51 69 85 61 95 79 47 54 51 59 12 45
[T2] Time took: 1012ms
246 271 232 437 116 407 238 104 41 280 158 374 431 150 80 343
[T3] Time took: 3950ms
[MAIN] Time took: 3951ms
*/