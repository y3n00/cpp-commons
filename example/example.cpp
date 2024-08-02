// everything was commented by AI, dont blame me

#include <fmt/chrono.h>
#include <fmt/format.h>
#include <fmt/ranges.h>

#include <iostream>
#include <list>
#include <set>

#define RANDOM_STATIC
#include "../console_colors.hpp"
#include "../random.hpp"
#include "../timer.hpp"
#include "../types.hpp"

static void test_timer() {
    std::cout << cliColors::ColorTxt::Colorize("[testing timer]", cliColors::Colors::red) << '\n';  // omg timer is watching u!!!
    static BenchTimer<Measurements::ms> global_timer;                                               // timer overlord ftw
    constexpr u16 max_length = 20, step = 5;                                                        // cuz who needs more than 20, amirite?

    for (auto current_len = max_length; current_len > 0; current_len -= step) {
        const auto& title_fmt = std::format("length {}", current_len);  // size matters, duh!
        Timer_Wrapper tw{global_timer.add(title_fmt)};                  // wrap it up like a burrito!

        const auto& str = Random_t::get_string(current_len);  // random string party like it's 1999!
        for (const auto& color : cliColors::getAllColors()) {
            const auto& colorized = cliColors::ColorTxt::Colorize(str, color);  // paint it rainbow, unicorns, and glitter!
            fmt::println("{}", colorized);                                      // show off, show off, show off!
        }
        std::cout << '\n';  // breathe, breathe, breathe...
    }

    for (const auto& [title, timer] : global_timer.get_all()) {
        fmt::println("{}:\t{}", title, timer.get_duration());  // timer's verdict: guilty of being awesome!
    }
}

static void test_random() {
    std::cout << cliColors::ColorTxt::Colorize("[testing random]", cliColors::Colors::red) << '\n';  // random incoming, hide your kids!
    u32 inp;
    std::cout << "enter size of vector: ";  // how big do u want it, bro?
    (std::cin >> inp).get();                // don't be shy, come on!

    const auto vec = Random_t::get_vector<i32>(inp);                                                     // random vector magic, abracadabra!
    fmt::println("vector<i32> with size: {}\nof random values {}\n", vec.size(), fmt::join(vec, ", "));  // ta-da, like a boss!

    const auto arr = Random_t::get_array<f32, 10>(-10.f, 10.f);                                         // random array party, woohoo!
    fmt::println("array<f32> with size: {}\nof random values {}\n", arr.size(), fmt::join(arr, ", "));  // woohoo, indeed!
}

static void test_random_api() {
    static Random_t rand;  // random master, aka the boss

    const auto v = rand.get_vector<uint32_t>(10, 0, 1000);   // random numbers, yay, party popper!
    fmt::print("random numbers: {}\n", fmt::join(v, ", "));  // look at them go, like a hot knife through butter!

    const auto a = rand.get_array<uint8_t, 16>();                                                                                   // random bytes, oh my, like a unicorn!
    fmt::print("random bytes: {}\n", fmt::join(a | std::views::transform([](auto i) { return std::format("{:#04x}", i); }), " "));  // hexy time, baby!

    const auto random_string = rand.get_string(10);    // random string, woohoo, like a firework!
    fmt::print("random string: {}\n", random_string);  // read it and weep, like a sad puppy!

    fmt::println("random integer in [-128, 127]: {}", rand.from_range<int8_t>());      // random int, yay, like a kid on christmas!
    fmt::println("random float in [0.0, 1.0]: {}", rand.from_range<float>(0.0, 1.0));  // random float, oh yeah, like a boss!

    std::vector<int> vec = {1, 2, 3, 4, 5};                     // the original gang, aka the cool kids
    rand.shuffle_range(vec);                                    // shuffle it like it's hot, like a dance party!
    fmt::print("shuffled vector: {}\n", fmt::join(vec, ", "));  // look at them now, like a proud parent!

    std::vector<int> vec2(10);                                // the new kids, aka the freshmen
    rand.fill_range(vec2, 0, 255);                            // fill 'er up, like a gas tank!
    fmt::print("filled range: {}\n", fmt::join(vec2, ", "));  // check it out, like a detective!

    std::set<int> from_set = {1, 2, 3, 4, 5};                         // the source, aka the original recipe
    std::list<int> to_list(5);                                        // the destination, aka the copycat
    rand.fill_range_from(to_list, from_set);                          // copy cat, like a xerox machine!
    fmt::print("filled range from: {}\n", fmt::join(to_list, ", "));  // the result, aka the clone!

    // std::vector<std::string> strings(10); failed
    // rand.fill_range(strings);

    auto floats = rand.get_vector<float>(10, 5.f, 10.f);
    fmt::print("random floats: {}\n", fmt::join(floats, ", "));

    // std::vector<std::string> strings(10); failed
    // rand.fill_range_from(strings, floats);
    std::vector<int> ints(10);
    rand.fill_range_from(ints, floats);
    fmt::print("filled range from: {}\n", fmt::join(ints, ", "));
}

int main() {
    test_timer();       // timer time, aka the countdown!
    test_random();      // randomness ensues, aka the chaos!
    test_random_api();  // random api party, aka the rave!
}