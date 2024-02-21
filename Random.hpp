#pragma once
#include <limits>
#include <random>
#include <string>

template <std::integral Ty>
class Random_t {
    using typeLimit = std::numeric_limits<Ty>;

   private:
    std::mt19937 gen{std::random_device{}()};

   public:
    Random_t() = default;
    Random_t(Random_t&&) = delete;
    Random_t(const Random_t&) = delete;
    Random_t& operator=(const Random_t&) = delete;

    [[nodiscard]] inline Ty get(Ty min = typeLimit::min(), Ty max = typeLimit::max()) {
        return std::uniform_int_distribution(min, max)(gen);
    }

    [[nodiscard]] inline std::string generate_string(size_t strLen) {
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
using Random = Random_t<uint32_t>;