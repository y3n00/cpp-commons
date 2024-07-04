#include <fmt/chrono.h>  //
#include <fmt/format.h>  //
#include <fmt/ranges.h>  // for convenient printing

#include <iostream>

#define RANDOM_STATIC
#include "../console_colors.hpp"
#include "../random.hpp"
#include "../timer.hpp"
#include "../types.hpp"

static void test_timer() {
    std::cout << cliColors::ColorTxt::Colorize("[Testing timer]", cliColors::Colors::red) << '\n';  // func title

    static BenchTimer<Measurements::ms> global_timer;  // from timer.hpp && measurements = aliases for std::chrono durations
    constexpr u16 max_length = 20, step = 5;           // u16 from types.hpp

    for (auto current_len = max_length; current_len > 0; current_len -= step) {
        const auto& title_fmt = std::format("length: {}", current_len);
        Timer_Wrapper tw{global_timer.add(title_fmt)};  // from timer.hpp && wrap Timer<T>& which the add() func returns

        const auto& str = Random_t<i16>::generate_string(current_len);          // random from random.hpp && i16 from types.hpp
        for (const auto& color : cliColors::getAllColors()) {                   // cliColors namespace from console_colors.hpp
            const auto& colorized = cliColors::ColorTxt::Colorize(str, color);  //
            fmt::println("{}", colorized);
        }
        std::cout << '\n';
    }  // tw destructor is somewhere here -> the timer at [title_fmt] stops

    for (const auto& [title, timer] : global_timer.get_all()) {  // get_all() returns an unordered_map, so we instantly use key and value
        fmt::println("{}:\t{}", title, timer.get_duration());    // get_duration() returns diff between stop and start in global_timer template param
    }
}

static void test_random() {
    std::cout << cliColors::ColorTxt::Colorize("[Testing random]", cliColors::Colors::red) << '\n';  // func title

    u32 inp;                                // for vec size input
    std::cout << "Enter size of vector: ";  // input ask
    (std::cin >> inp).get();                //

    const auto vec = Random_t<i32>::filled_vector(inp);                                                // get vector by size
    fmt::println("Vector<i32> with size: {}\nof random values {}", vec.size(), fmt::join(vec, ", "));  // print vec
    std::cout << '\n';
    const auto arr = Random_t<f32>::filled_array<10>(0.f, 1.f);                                       // get arr by sz and min max values
    fmt::println("Array<f32> with size: {}\nof random values {}", arr.size(), fmt::join(arr, ", "));  // print arr
}

int main() {
    test_timer();
    test_random();
}