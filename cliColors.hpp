#pragma once
#include <array>
#include <string>

namespace cliColors {
enum Colors : uint8_t {
    red = 0,
    orange,
    yellow,
    green,
    cyan,
    blue,
    purple,
    pink,
    brightgreen,
    brightred,
    black,
    gray,
    brightgray,
    white,
    reset,
    _default,
    SIZE
};

class ColorTxt {
   public:
    static inline const std::string Colorize(const std::string& str, Colors c) {
        return m_colors[c] + str + m_colors[reset];
    }

    static inline const std::string GetColor(Colors color) { return m_colors[color]; }
    static inline const std::string Reset() { return m_colors[reset]; }

   private:
    static const inline std::array<const char*, Colors::SIZE> m_colors{
        "\e[1;31m", "\e[1;33m",
        "\e[1;93m", "\e[1;32m",
        "\e[1;36m", "\e[1;34m",
        "\e[1;35m", "\e[1;95m",
        "\e[1;92m", "\e[1;91m",
        "\e[1;30m", "\e[1;90m",
        "\e[1;37m", "\e[1;97m",
        "\e[0m", ""};
};
};  // namespace cliColors

// namespace cliColors
