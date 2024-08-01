#pragma once

#include <array>
#include <concepts>
#include <execution>
#include <limits>
#include <optional>
#include <random>
#include <string>
#include <vector>

#ifdef RANDOM_STATIC
#define IF_STATIC static
#define IF_STATIC_VAR static inline
#else
#define IF_STATIC
#define IF_STATIC_VAR
#endif

template <typename T>
concept Numeric_Type = std::is_arithmetic_v<T>;

template <Numeric_Type Num_Type>
class Random_t {
    using Type_Limit = std::numeric_limits<Num_Type>;

   private:
    thread_local IF_STATIC_VAR std::mt19937 gen{std::random_device{}()};

   public:
    Random_t() = default;
    Random_t(Random_t&&) = delete;
    Random_t(const Random_t&) = delete;
    Random_t& operator=(const Random_t&) = delete;

    [[nodiscard]] IF_STATIC inline Num_Type from_range(Num_Type min_val = Type_Limit::min(), Num_Type max_val = Type_Limit::max()) {
        if constexpr (std::is_floating_point_v<Num_Type>) {
            return std::uniform_real_distribution<Num_Type>{min_val, max_val}(gen);
        } else {
            if constexpr (sizeof(Num_Type) == 1) {
                const auto temp_value = std::uniform_int_distribution<int16_t>{min_val, max_val}(gen);
                return static_cast<Num_Type>(temp_value);
            } else {
                return std::uniform_int_distribution<Num_Type>{min_val, max_val}(gen);
            }
        }
    }

    [[nodiscard]] IF_STATIC inline Num_Type from_zero_to(Num_Type max_val = Type_Limit::max()) {
        return from_range(Num_Type{}, max_val);
    }

    template <std::ranges::random_access_range R>
    [[nodiscard]] IF_STATIC inline auto get_elem(R&& range) {
        using ret_t = std::optional<std::ranges::range_value_t<R>>;

        thread_local static Random_t<size_t> rng;
        const auto size = std::ranges::size(range);
        return size ? ret_t{range[rng.from_zero_to(size - 1)]} : ret_t{};
    }

    template <std::ranges::random_access_range R>
    [[nodiscard]] IF_STATIC inline void fill_range(R& range, Num_Type min_val = Type_Limit::min(), Num_Type max_val = Type_Limit::max()) {
        using value_type = std::ranges::range_value_t<R>;
        static_assert(std::is_integral_v<value_type>);

        thread_local static Random_t<value_type> rng;
        std::ranges::for_each(range, [&](auto& elem) { elem = rng.from_range(min_val, max_val); });
    }

    template <std::ranges::random_access_range R>
    [[nodiscard]] IF_STATIC inline void fill_range_from(R& range, std::ranges::random_access_range auto&& from) {
        if (std::ranges::empty(from))
            return;

        thread_local static Random_t<std::ranges::range_value_t<R>> rng;
        std::ranges::for_each(range, [&](auto& elem) {
            elem = rng.get_elem(from).value();
        });
    }

    [[nodiscard]] IF_STATIC inline decltype(auto) get_string(size_t str_len, const std::string& extra_chars = "") {
        constexpr std::string_view basic_symbols =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "1234567890";
        std::string result(str_len, ' ');

        const auto final_range = extra_chars + basic_symbols.data();
        fill_range_from(result, final_range);
        return result;
    }

    template <size_t SZ>
    [[nodiscard]] IF_STATIC inline decltype(auto) get_array(Num_Type min_val = Type_Limit::min(), Num_Type max_val = Type_Limit::max()) {
        std::array<Num_Type, SZ> arr;
        fill_range(arr, min_val, max_val);
        return arr;
    }

    [[nodiscard]] IF_STATIC inline decltype(auto) get_vector(size_t size, Num_Type min_val = Type_Limit::min(), Num_Type max_val = Type_Limit::max()) {
        std::vector<Num_Type> vec(size);
        fill_range(vec, min_val, max_val);
        return vec;
    }
};
