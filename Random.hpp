#pragma once
#include <concepts>
#include <limits>
#include <random>

#ifdef RANDOM_STRING
#include <string>
#endif

#ifdef RANDOM_STATIC
#define IF_STATIC static
#define IF_STATIC_VAR static inline
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

    [[nodiscard]] IF_STATIC inline NT get(NT min = typeLimit::min(), NT max = typeLimit::max()) {
        if constexpr (std::is_floating_point_v<NT>)
            return std::uniform_real_distribution<NT>{min, max}(gen);
        else
            return std::uniform_int_distribution{min, max}(gen);
    }

#ifdef RANDOM_STRING
    [[nodiscard]] IF_STATIC inline std::string generate_string(size_t strLen) {
        constexpr static std::string_view SYMBOLS =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "1234567890";
        std::string result(strLen, ' ');
        for (char& ch : result)
            ch = SYMBOLS[get(0, SYMBOLS.length() - 1)];
        return result;
    }
#endif
};
using Random = Random_t<uint32_t>;