// !everything was commented by AI, dont blame me

#include <format>
#include <iostream>
#include <list>
#include <print>
#include <ranges>
#include <set>

#define RANDOM_STATIC
#include "../console_colors.hpp"
#include "../random.hpp"
#include "../timer.hpp"

static inline auto print_title(std::string_view title) {
    std::println("\n{}", cliColors::ColorTxt::Colorize(title, Random_t::get_elem(cliColors::getAllColors())));
}

template <typename... Args>
static inline void run_times(size_t count, auto&& f, Args&&... args) {
    while (count--) {
        f(std::forward<Args>(args)...);
    }
}

#pragma optimize("", off)  // cuz compiler optimize unused generated strings
static void test_timer() {
    print_title("[testing timer]");

    BenchTimer<Measurements::ms> global_timer;  // timer overlord ftw
    int16_t curr_length = 10000, step = 100;    // cuz who needs more than 20, amirite?
    {
        ScopeTimer _{global_timer.add("Total time")};  // wrap it up like a burrito!

        for (; curr_length > 0; curr_length -= step) {
            const auto title = std::format("length {}", curr_length);  // size matters, duh!
            ScopeTimer _{global_timer.add(title)};                     // wrap it up like a burrito!
            const auto& str = Random_t::get_string(curr_length);       // random string party like it's 1999!
        }

        {
            constexpr static auto vec_sz = static_cast<size_t>(10000);
            const auto title = std::format("generation {} random strings", vec_sz);
            ScopeTimer _{global_timer.add(title)};
            static const auto& vec_of_strings = Random_t::get_string_vec(vec_sz);
        }
    }

    for (const auto& [title, timer] : global_timer.get_all()) {
        std::println("{}:\t{}", title, timer.get_duration());  // timer's verdict: guilty of being awesome!
    }
}
#pragma optimize("", on)

static void test_random_api(bool by_same_seed = true) {
    print_title("[testing random api]");

    auto rand = by_same_seed ? Random_t{1337} : Random_t{};  // random master, aka the boss

    std::println("random numbers: {}", rand.get_numeric_vector<uint32_t>(10, 0, 1000));  // look at them go, like a hot knife through butter!

    std::println("random bytes: {}", rand.get_numeric_array<uint8_t, 16>() | std::views::transform([](auto i) { return std::format("{:#04x}", i); }));  // hexy time, baby!

    std::println("random string: {}", rand.get_string(10));  // read it and weep, like a sad puppy!

    std::println("random integer in [-128, 127]: {}", rand.in_range<int8_t>());      // random int, yay, like a kid on christmas!
    std::println("random float in [0.0, 1.0]: {}", rand.in_range<float>(0.0, 1.0));  // random float, oh yeah, like a boss!

    std::vector<int> vec = {1, 2, 3, 4, 5};    // the original gang, aka the cool kids
    rand.shuffle_range(vec);                   // shuffle it like it's hot, like a dance party!
    std::println("shuffled vector: {}", vec);  // look at them now, like a proud parent!

    std::vector<int> vec2(10);               // the new kids, aka the freshmen
    rand.fill_range(vec2, 0, 255);           // fill 'er up, like a gas tank!
    std::println("filled range: {}", vec2);  // check it out, like a detective!

    std::set<int> from_set = {1, 2, 3, 4, 5};        // the source, aka the original recipe
    std::list<int> to_list(5);                       // the destination, aka the copycat
    rand.fill_range_from(to_list, from_set);         // copy cat, like a xerox machine!
    std::println("filled range from: {}", to_list);  // the result, aka the clone!

    auto floats = rand.get_numeric_vector<float>(10, 5.f, 10.f);
    std::println("random floats: {}", floats);

    std::vector<int> ints(10);
    rand.fill_range_from(ints, floats);
    std::println("filled range from: {}", ints);

    std::println("10 random bools {}", rand.get_numeric_array<bool, 10>());
}

static void check_color_inc() noexcept {
    using namespace cliColors;
    print_title("[testing color increment]");

    Colors color = Colors::black;
    constexpr auto msg = std::string_view{"hello world"};
    for (size_t i = 0; i < 30; i++) {
        std::println("{}", ColorTxt::Colorize(msg, color++));
    }
}

int main() {
    run_times(1, test_timer);
    run_times(3, test_random_api, false);  // several times to check different values
    run_times(3, test_random_api, true);   // several times to be sure about generating by seed
    run_times(1, check_color_inc);
}

/*
Possible output:

[testing timer]
length 500:     0.0044ms
length 5400:    0.041ms
length 9800:    0.075ms
length 8700:    0.0666ms
Total time:     19.4998ms
length 4700:    0.0364ms
length 900:     0.0072ms
length 5800:    0.0444ms
length 9400:    0.0719ms
length 6500:    0.0494ms
length 10000:   0.0826ms
length 400:     0.0039ms
length 5500:    0.0424ms
length 9900:    0.0752ms
length 4400:    0.0338ms
length 9700:    0.0745ms
length 4500:    0.0345ms
length 9600:    0.0747ms
length 4600:    0.0351ms
length 800:     0.0064ms
length 5900:    0.0456ms
length 9500:    0.0724ms
length 4000:    0.0347ms
length 9300:    0.0711ms
length 4100:    0.0361ms
length 9200:    0.0706ms
length 4200:    0.0325ms
length 9100:    0.0699ms
length 4300:    0.0331ms
length 9000:    0.0684ms
length 3800:    0.0296ms
length 8900:    0.0707ms
length 3900:    0.0316ms
length 8800:    0.0743ms
length 8600:    0.0658ms
length 8500:    0.0652ms
length 8400:    0.0639ms
length 8300:    0.0632ms
generation 10000 random strings:        15.4976ms
length 8200:    0.0632ms
length 8100:    0.0642ms
length 8000:    0.0606ms
length 7900:    0.0606ms
length 7800:    0.06ms
length 7700:    0.0602ms
length 7600:    0.0581ms
length 7500:    0.0574ms
length 7400:    0.0607ms
length 7300:    0.0563ms
length 7200:    0.0546ms
length 7100:    0.0548ms
length 7000:    0.0533ms
length 6900:    0.0527ms
length 6800:    0.0521ms
length 6700:    0.0508ms
length 6600:    0.0508ms
length 6400:    0.0495ms
length 6300:    0.0534ms
length 6200:    0.0482ms
length 6100:    0.0469ms
length 6000:    0.0456ms
length 4800:    0.037ms
length 600:     0.005ms
length 5700:    0.0436ms
length 4900:    0.0377ms
length 700:     0.0058ms
length 5600:    0.043ms
length 200:     0.0024ms
length 5300:    0.041ms
length 300:     0.0025ms
length 5200:    0.0396ms
length 5100:    0.0391ms
length 100:     0.001ms
length 5000:    0.0384ms
length 3700:    0.0336ms
length 3600:    0.0284ms
length 3500:    0.0269ms
length 3400:    0.0269ms
length 3300:    0.0256ms
length 3200:    0.0251ms
length 3100:    0.0243ms
length 3000:    0.0236ms
length 2900:    0.0231ms
length 2800:    0.0217ms
length 2700:    0.0217ms
length 2600:    0.0203ms
length 2500:    0.0196ms
length 2400:    0.0189ms
length 2300:    0.0181ms
length 2200:    0.0177ms
length 2100:    0.0163ms
length 2000:    0.0157ms
length 1900:    0.015ms
length 1800:    0.0143ms
length 1700:    0.0136ms
length 1600:    0.0144ms
length 1500:    0.0118ms
length 1400:    0.0111ms
length 1300:    0.0104ms
length 1200:    0.0097ms
length 1100:    0.009ms
length 1000:    0.0085ms

[testing random api]
random numbers: [462, 418, 537, 797, 765, 881, 221, 827, 816, 291]
random bytes: ["0x48", "0xe9", "0x38", "0x6f", "0xf3", "0x43", "0x48", "0xd8", "0x2a", "0xfd", "0x4b", "0x1b", "0xbc", "0x1e", "0x7b", "0xef"]
random string: Ws8Qm2zDLE
random integer in [-128, 127]: 14
random float in [0.0, 1.0]: 0.89706045
shuffled vector: [4, 2, 1, 5, 3]
filled range: [47, 161, 54, 122, 117, 230, 96, 120, 97, 201]
filled range from: [5, 2, 5, 5, 4]
random floats: [8.235565, 9.669035, 9.536044, 9.814918, 5.7849126, 9.45631, 9.350123, 8.635424, 5.902574, 8.284173]
filled range from: [5, 9, 5, 5, 9, 8, 8, 9, 8, 8]
10 random bools [false, false, true, true, true, false, false, true, true, false]

[testing random api]
random numbers: [379, 525, 582, 266, 290, 248, 121, 244, 990, 266]
random bytes: ["0xc1", "0x98", "0x4e", "0x5e", "0x1d", "0x0c", "0xf7", "0x57", "0x2e", "0xa6", "0xea", "0x66", "0x37", "0x07", "0x03", "0x9a"]
random string: Ff7etQJnLm
random integer in [-128, 127]: 3
random float in [0.0, 1.0]: 0.6390263
shuffled vector: [1, 5, 2, 3, 4]
filled range: [153, 219, 241, 145, 87, 182, 31, 249, 244, 122]
filled range from: [5, 2, 2, 2, 5]
random floats: [6.259288, 8.763029, 8.504117, 5.1901608, 8.337881, 5.842333, 6.438217, 7.513108, 5.7693634, 8.538461]
filled range from: [5, 8, 8, 7, 8, 7, 6, 5, 5, 5]
10 random bools [true, true, true, true, true, true, false, true, true, true]

[testing random api]
random numbers: [30, 879, 116, 148, 160, 58, 183, 212, 994, 60]
random bytes: ["0xe9", "0x2a", "0x4b", "0x10", "0xb4", "0xf6", "0x36", "0xd1", "0x57", "0xda", "0x72", "0x5e", "0x2b", "0x48", "0xc7", "0xb5"]
random string: BzCa3BWSuO
random integer in [-128, 127]: 11
random float in [0.0, 1.0]: 0.8406246
shuffled vector: [1, 2, 4, 5, 3]
filled range: [185, 29, 130, 178, 191, 152, 64, 50, 155, 51]
filled range from: [2, 1, 1, 4, 2]
random floats: [9.967522, 8.300336, 5.030487, 9.422998, 6.862858, 5.5313325, 6.653205, 7.8751745, 7.4313803, 6.5837984]
filled range from: [7, 6, 6, 6, 5, 5, 7, 7, 6, 7]
10 random bools [true, true, false, true, false, true, false, false, false, true]

[testing random api]
random numbers: [262, 561, 158, 212, 278, 543, 459, 34, 321, 235]
random bytes: ["0x84", "0x97", "0x43", "0x55", "0xf9", "0x45", "0xbb", "0x4e", "0x1d", "0xcd", "0x62", "0x18", "0xa0", "0x5c", "0x20", "0xbd"]
random string: 9Jb2wwxAWc
random integer in [-128, 127]: -22
random float in [0.0, 1.0]: 0.38106865
shuffled vector: [4, 2, 3, 5, 1]
filled range: [48, 121, 73, 221, 171, 103, 127, 112, 45, 36]
filled range from: [3, 5, 1, 5, 3]
random floats: [9.6420355, 9.161854, 8.097736, 5.9262547, 9.787113, 9.786796, 9.405002, 7.1270733, 9.067889, 7.520035]
filled range from: [9, 9, 9, 9, 9, 7, 9, 7, 9, 9]
10 random bools [false, false, false, false, true, true, true, false, true, false]

[testing random api]
random numbers: [262, 561, 158, 212, 278, 543, 459, 34, 321, 235]
random bytes: ["0x84", "0x97", "0x43", "0x55", "0xf9", "0x45", "0xbb", "0x4e", "0x1d", "0xcd", "0x62", "0x18", "0xa0", "0x5c", "0x20", "0xbd"]
random string: 9Jb2wwxAWc
random integer in [-128, 127]: -22
random float in [0.0, 1.0]: 0.38106865
shuffled vector: [4, 2, 3, 5, 1]
filled range: [48, 121, 73, 221, 171, 103, 127, 112, 45, 36]
filled range from: [3, 5, 1, 5, 3]
random floats: [9.6420355, 9.161854, 8.097736, 5.9262547, 9.787113, 9.786796, 9.405002, 7.1270733, 9.067889, 7.520035]
filled range from: [9, 9, 9, 9, 9, 7, 9, 7, 9, 9]
10 random bools [false, false, false, false, true, true, true, false, true, false]

[testing random api]
random numbers: [262, 561, 158, 212, 278, 543, 459, 34, 321, 235]
random bytes: ["0x84", "0x97", "0x43", "0x55", "0xf9", "0x45", "0xbb", "0x4e", "0x1d", "0xcd", "0x62", "0x18", "0xa0", "0x5c", "0x20", "0xbd"]
random string: 9Jb2wwxAWc
random integer in [-128, 127]: -22
random float in [0.0, 1.0]: 0.38106865
shuffled vector: [4, 2, 3, 5, 1]
filled range: [48, 121, 73, 221, 171, 103, 127, 112, 45, 36]
filled range from: [3, 5, 1, 5, 3]
random floats: [9.6420355, 9.161854, 8.097736, 5.9262547, 9.787113, 9.786796, 9.405002, 7.1270733, 9.067889, 7.520035]
filled range from: [9, 9, 9, 9, 9, 7, 9, 7, 9, 9]
10 random bools [false, false, false, false, true, true, true, false, true, false]

[testing color increment]
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world
hello world

*/