#pragma once

#include <array>
#include <concepts>
#include <limits>
#include <random>
#include <ranges>
#include <string>
#include <vector>

#ifdef RANDOM_STATIC
#define AUTO_SIGNATURE inline static auto
#define VARIABLE_TYPE static inline thread_local
#else
#define AUTO_SIGNATURE inline auto
#define VARIABLE_TYPE
#endif

template <typename T>
concept Numeric_Type = std::is_arithmetic_v<T>;

#define CREATE_LIMIT(T, name, val) T name = ((std::numeric_limits<T>::val)())

#define MIN_MAX_LIMIT(T, op) CREATE_LIMIT(T, op##_val, op)

#define MAX_LIMIT(T) MIN_MAX_LIMIT(T, max)
#define MIN_LIMIT(T) MIN_MAX_LIMIT(T, min)

class Random_t {
    template <std::integral I>
    using int_dist = std::uniform_int_distribution<I>;

    template <std::floating_point R>
    using real_dist = std::uniform_real_distribution<R>;

   private:
    VARIABLE_TYPE std::mt19937_64 gen{std::random_device{}()};

    template <Numeric_Type Num_Type>
    [[nodiscard]] constexpr AUTO_SIGNATURE get_distribution(MIN_LIMIT(Num_Type), MAX_LIMIT(Num_Type)) noexcept {
        if constexpr (std::is_floating_point_v<Num_Type>)
            return real_dist<Num_Type>{min_val, max_val};
        else if constexpr (sizeof(Num_Type) == 1)        // cant use 1 byte types on msvc
            return int_dist<int16_t>{min_val, max_val};  // int16_t covers all numeric limits of 1 byte types
        else
            return int_dist<Num_Type>{min_val, max_val};
    }

   public:
    Random_t() = default;
    Random_t(size_t seed) noexcept { gen.seed(seed); };
    Random_t(Random_t&&) = delete;
    Random_t(const Random_t&) = delete;
    Random_t& operator=(const Random_t&) = delete;

    template <Numeric_Type Num_Type>
    [[nodiscard]] AUTO_SIGNATURE in_range(MIN_LIMIT(Num_Type),
                                          MAX_LIMIT(Num_Type)) noexcept -> Num_Type {
        return static_cast<Num_Type>(get_distribution(min_val, max_val)(gen));
    }

    template <Numeric_Type Num_Type>
    [[nodiscard]] AUTO_SIGNATURE from_zero_to(MAX_LIMIT(Num_Type)) noexcept -> Num_Type {
        return in_range<Num_Type>({}, max_val);
    }

    [[nodiscard]] AUTO_SIGNATURE get_elem(std::ranges::range auto&& range) noexcept {
        const auto last_idx = std::ranges::size(range) - 1;
        auto it = std::ranges::begin(range);
        std::ranges::advance(it, from_zero_to(last_idx));
        return *it;
    }

    template <std::ranges::range R,
              typename T = std::ranges::range_value_t<R>>
        requires Numeric_Type<T>
    AUTO_SIGNATURE fill_range(R& range,
                              MIN_LIMIT(T),
                              MAX_LIMIT(T)) noexcept -> void {
        std::ranges::for_each(range, [&](auto& elem) {
            elem = in_range<T>(min_val, max_val);
        });
    }

    template <std::ranges::range R, Numeric_Type len_t = uint8_t>
        requires std::is_same_v<std::ranges::range_value_t<R>, std::string>
    AUTO_SIGNATURE fill_range(R& range,
                              len_t min_len,
                              len_t max_len,
                              std::string extra_chars = "") noexcept -> void {
        std::ranges::for_each(range, [&](auto& elem) {
            elem = get_string(in_range(min_len, max_len), extra_chars);
        });
    }

    template <std::ranges::range R1,
              std::ranges::range R2,
              typename T1 = std::ranges::range_value_t<R1>,
              typename T2 = std::ranges::range_value_t<R2>>
        requires std::same_as<T1, T2> || std::convertible_to<T2, T1>
    AUTO_SIGNATURE fill_range_from(R1& to_fill,
                                   R2&& fill_from) noexcept -> void {
        if (not std::ranges::empty(fill_from))
            std::ranges::for_each(to_fill, [&](auto& elem) {
                elem = static_cast<T1>(get_elem(fill_from));
            });
    }

    AUTO_SIGNATURE shuffle_range(std::ranges::random_access_range auto& range) -> void {
        std::ranges::shuffle(range, gen);
    }

    [[nodiscard]] AUTO_SIGNATURE get_string(size_t str_len,
                                            std::string extra_chars = "") noexcept -> std::string {
        constexpr static std::string_view basic_symbols =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "1234567890";
        std::string result(str_len, '.');
        fill_range_from(result, extra_chars + basic_symbols.data());
        return result;
    }

    template <Numeric_Type Num_Type, size_t SZ>
    [[nodiscard]] AUTO_SIGNATURE get_numeric_array(MIN_LIMIT(Num_Type),
                                                   MAX_LIMIT(Num_Type)) noexcept -> std::array<Num_Type, SZ> {
        std::array<Num_Type, SZ> arr;
        fill_range(arr, min_val, max_val);
        return arr;
    }

    template <size_t SZ, Numeric_Type len_t = uint8_t>  // foolproofing
    [[nodiscard]] AUTO_SIGNATURE get_string_array(MIN_LIMIT(len_t),
                                                  MAX_LIMIT(len_t)) noexcept -> std::array<std::string, SZ> {
        std::array<std::string, SZ> arr;
        fill_range(arr, min_val, max_val);
        return arr;
    }

    template <Numeric_Type Num_Type>
    [[nodiscard]] AUTO_SIGNATURE get_numeric_vector(size_t size,
                                                    MIN_LIMIT(Num_Type),
                                                    MAX_LIMIT(Num_Type)) noexcept -> std::vector<Num_Type> {
        std::vector<Num_Type> vec(size);
        fill_range(vec, min_val, max_val);
        return vec;
    }

    template <Numeric_Type len_t = uint8_t>  // foolproofing
    [[nodiscard]] AUTO_SIGNATURE get_string_vector(size_t size,
                                                   CREATE_LIMIT(len_t, min_str_length, min),
                                                   CREATE_LIMIT(len_t, max_str_length, max),
                                                   std::string extra_chars = "") noexcept -> std::vector<std::string> {
        std::vector<std::string> vec(size);
        fill_range(vec, min_str_length, max_str_length, std::move(extra_chars));
        return vec;
    }

    [[nodiscard]] AUTO_SIGNATURE get_bool() noexcept -> bool {
        return from_zero_to<bool>();
    }

    [[nodiscard]] AUTO_SIGNATURE chance_probability(double prob) noexcept -> bool {
        return std::bernoulli_distribution{std::clamp<double>(prob, 0.f, 1.f)}(gen);
    }

    [[nodiscard]] AUTO_SIGNATURE chance_percent(double percent) noexcept -> bool {
        return chance_probability(percent / 100);
    }
};

#undef AUTO_SIGNATURE
#undef VARIABLE_TYPE
#undef CREATE_LIMIT
#undef MIN_MAX_LIMIT
#undef MAX_LIMIT
#undef MIN_LIMIT