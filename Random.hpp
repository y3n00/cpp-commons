#pragma once
#include <limits>
#include <random>
#include <string>

template <std::integral Ty = uint32_t>
class Random {
    using typeLimit = std::numeric_limits<Ty>;

   private:
    static inline std::mt19937 gen{std::random_device{}()};

   public:
    Random() = default;
    Random(Random&&) = delete;
    Random(const Random&) = delete;
    Random& operator=(const Random&) = delete;

    [[nodiscard]] static inline Ty get(Ty min = typeLimit::min(), Ty max = typeLimit::max()) {
        return std::uniform_int_distribution(min, max)(gen);
    }

    [[nodiscard]] static inline std::string generate_string(size_t strLen) {
        constexpr static std::string_view SYMBOLS =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "1234567890";
        std::string result(strLen, ' ');
        for (char& ch : result)
            ch = SYMBOLS[get(0, SYMBOLS.length() - 1)];
        return result;
    }
};