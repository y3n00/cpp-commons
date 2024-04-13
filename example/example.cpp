#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include <iostream>

#define RANDOM_STATIC
#include "../console_colors.hpp"
#include "../random.hpp"
#include "../timer.hpp"
#include "../types.hpp"

int main() {
    static BenchTimer<Measurements::ms> global_timer;
    constexpr auto max_length = 50;
    constexpr auto step = 5;

    for (auto current_len = max_length; current_len > 0; current_len -= step) {
        const auto& title_fmt = std::format("length: {}", current_len);
        Timer_Wrapper tw{global_timer.add(title_fmt)};  // wrap Timer<T>& which the add() func returns

        const auto& str = Random_t<i16>::generate_string(current_len);
        for (const auto& color : cliColors::getAllColors()) {
            const auto& colorized = cliColors::ColorTxt::Colorize(str, color);
            fmt::println("{}", colorized);
        }
        std::cout << '\n';
    }  // tw destructor is somewhere here -> the timer at [title_fmt] stops

    for (const auto& [title, timer] : global_timer.get_all()) {
        fmt::println("{}:\t{}", title, timer.get_duration());
    }
}