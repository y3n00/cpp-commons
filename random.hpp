#pragma once

#include <array>
#include <concepts>
#include <limits>
#include <random>
#include <string>
#include <vector>

#ifdef RANDOM_STATIC
#define IF_STATIC static
#define IF_STATIC_VAR static inline thread_local
#else
#define IF_STATIC
#define IF_STATIC_VAR
#endif

template <typename T>
concept NumericType = std::integral<T> || std::floating_point<T>;

template <NumericType NT>
class Random_t {
    using typeLimit = std::numeric_limits<NT>;

   private:
    IF_STATIC_VAR std::mt19937 gen{std::random_device{}()};

   public:
    Random_t() = default;
    Random_t(Random_t&&) = delete;
    Random_t(const Random_t&) = delete;
    Random_t& operator=(const Random_t&) = delete;

    [[nodiscard]] IF_STATIC NT get(NT min_val = typeLimit::min(), NT max_val = typeLimit::max()) {
        if constexpr (std::is_floating_point_v<NT>) {
            return std::uniform_real_distribution<NT>{min_val, max_val}(gen);
        } else {
            if constexpr (sizeof(NT) == 1) {
                const auto v = std::uniform_int_distribution<int16_t>{min_val, max_val}(gen);
                return static_cast<NT>(v);
            } else {
                return std::uniform_int_distribution<NT>{min_val, max_val}(gen);
            }
        }
    }

    [[nodiscard]] IF_STATIC std::string generate_string(size_t strLen) {
        constexpr static std::string_view SYMBOLS =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "1234567890";
        constexpr static auto last_idx = static_cast<NT>(SYMBOLS.length() - 1);
        std::string result(strLen, ' ');
        for (char& ch : result)
            ch = SYMBOLS[get(0, last_idx)];
        return result;
    }

    template <std::ranges::range R>
    [[nodiscard]] IF_STATIC void fill_range(R& range, NT min_val = typeLimit::min(), NT max_val = typeLimit::max()) {
        for (auto& element : range) {
            element = get(min_val, max_val);
        }
    }

    template <std::size_t SZ>
    [[nodiscard]] IF_STATIC std::array<NT, SZ> filled_array(NT min = typeLimit::min(), NT max = typeLimit::max()) {
        std::array<NT, SZ> arr;
        fill_range(arr, min, max);
        return arr;
    }

    [[nodiscard]] IF_STATIC std::vector<NT> filled_vector(std::size_t size, NT min_val = typeLimit::min(), NT max_val = typeLimit::max()) {
        std::vector<NT> vec(size);
        fill_range(vec, min_val, max_val);
        return vec;
    }
};