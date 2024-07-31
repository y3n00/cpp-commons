#pragma once

#include <array>
#include <concepts>
#include <execution>
#include <limits>
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
concept Numeric_Type = std::integral<T> || std::floating_point<T>;

template <Numeric_Type NT>
class Random_t {
    using Type_Limit = std::numeric_limits<NT>;

   private:
    constexpr static inline std::string_view m_all_symbols =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "1234567890";
    thread_local IF_STATIC_VAR std::mt19937 gen{std::random_device{}()};

   public:
    Random_t() = default;
    Random_t(Random_t&&) = delete;
    Random_t(const Random_t&) = delete;
    Random_t& operator=(const Random_t&) = delete;

    [[nodiscard]] IF_STATIC inline NT get(NT min_val = Type_Limit::min(), NT max_val = Type_Limit::max()) {
        if constexpr (std::is_floating_point_v<NT>) {
            return std::uniform_real_distribution<NT>{min_val, max_val}(gen);
        } else {
            if constexpr (sizeof(NT) == 1) {
                const auto temp_value = std::uniform_int_distribution<int16_t>{min_val, max_val}(gen);
                return static_cast<NT>(temp_value);
            } else {
                return std::uniform_int_distribution<NT>{min_val, max_val}(gen);
            }
        }
    }

    [[nodiscard]] IF_STATIC inline NT get(NT max_val = Type_Limit::max()) {
        constexpr static NT min_val = 0;
        if constexpr (std::is_floating_point_v<NT>) {
            return std::uniform_real_distribution<NT>{min_val, max_val}(gen);
        } else {
            if constexpr (sizeof(NT) == 1) {
                const auto temp_value = std::uniform_int_distribution<int16_t>{min_val, max_val}(gen);
                return static_cast<NT>(temp_value);
            } else {
                return std::uniform_int_distribution<NT>{min_val, max_val}(gen);
            }
        }
    }

    template <std::ranges::random_access_range R>
    [[nodiscard]] IF_STATIC inline const auto& get(const R& range) {
        static_assert(std::is_integral_v<NT>);
        using value_type = std::ranges::range_value_t<R>;

        const auto sz = std::ranges::size(range);
        return sz > 0 ? range[get(0, sz - 1)] : value_type{};
    }

    [[nodiscard]] IF_STATIC inline std::string generate_string(size_t str_len) {
        constexpr static auto last_idx = static_cast<NT>(m_all_symbols.length() - 1);
        std::string result(str_len, ' ');
        for (char& ch : result)
            ch = m_all_symbols[get(0, last_idx)];
        return result;
    }

    [[nodiscard]] IF_STATIC inline std::string generate_string(size_t str_len, const std::string& extra_chars) {
        const auto all_symbols = extra_chars + m_all_symbols.data();
        const auto last_idx = static_cast<NT>(all_symbols.length() - 1);
        std::string result(str_len, ' ');
        for (char& ch : result)
            ch = all_symbols[get(0, last_idx)];
        return result;
    }

    [[nodiscard]] IF_STATIC inline void fill_range(std::ranges::range auto& range, NT min_val = Type_Limit::min(), NT max_val = Type_Limit::max()) {
        using value_type = std::ranges::range_value_t<R>;
        static_assert(std::is_integral<value_type>);

        thread_local static Random_t<value_type> rng;
        std::ranges::for_each(range, [&](auto& elem) { elem = rng.get(min_val, max_val); });
    }

    template <size_t SZ>
    [[nodiscard]] IF_STATIC inline decltype(auto) get_array(NT min = Type_Limit::min(), NT max = Type_Limit::max()) {
        std::array<NT, SZ> arr;
        fill_range(arr, min, max);
        return arr;
    }

    [[nodiscard]] IF_STATIC inline decltype(auto) get_vector(size_t size, NT min_val = Type_Limit::min(), NT max_val = Type_Limit::max()) {
        std::vector<NT> vec(size);
        fill_range(vec, min_val, max_val);
        return vec;
    }
};