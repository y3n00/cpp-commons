#include <fmt/core.h>

#include "default.hpp"

struct S {
    S() {}
    S(const S& copy) { fmt::print("S copy ctor\n"); }
    S(S&& move) { fmt::print("S move ctor\n"); }
};

template <typename T>
struct SS {
    T operator()(std::initializer_list<T>&& values) {
        T tempVal{};
        for (auto&& v : values)
            tempVal += v;

        return tempVal;
    }
};

int main() {
    S s1;
    const S s2;

    S s3(s1);             // S copy ctor
    S s4(s2);             // S copy ctor
    S s5(std::move(s1));  // S move ctor
    S s6(std::move(s2));  // S copy ctor

    SS<int> si;
    SS<double> sd;

    fmt::print("{}\n{}\n",
               si({2, 123, 324, 42, 123, 123, 32, 5, 4323, 5454, 534}),
               sd({1.4532, 534.34, 456.4, 123.21342, 0.5437654, 4.67676756}));
}

/* OUTPUT
S copy ctor
S copy ctor
S move ctor
S copy ctor
11085
1120.6271529599999
*/
