#pragma once

#if defined(_MSVC_LANG)
#define CPP_STANDARD _MSVC_LANG
#else
#define CPP_STANDARD __cplusplus
#endif

#if (CPP_STANDARD >= 202002L)
#include <format>
namespace _FMT = std;
#else
#include <fmt/format.h>
namespace _FMT = fmt;
#endif

#include <string>
#include <vector>
namespace cliColors {
    enum class Colors {
        reset = 0,
        black = 30,
        red = 31,
        green = 32,
        yellow = 33,
        orange = 33,
        blue = 34,
        purple = 35,
        pink = 35,
        cyan = 36,
        gray = 37,
        brightgray = 90,
        darkgray = 90,
        brightred = 91,
        brightgreen = 92,
        lightblue = 94,
        magenta = 95,
        lightcyan = 96,
        white = 97,
        _default = 39,
    };

    static inline std::vector<Colors> getAllColors() {
        static const auto colors = std::vector{
            Colors::reset,
            Colors::black,
            Colors::red,
            Colors::green,
            Colors::yellow,
            Colors::orange,
            Colors::blue,
            Colors::purple,
            Colors::pink,
            Colors::cyan,
            Colors::gray,
            Colors::brightgray,
            Colors::darkgray,
            Colors::brightred,
            Colors::brightgreen,
            Colors::lightblue,
            Colors::magenta,
            Colors::lightcyan,
            Colors::white,
            Colors::_default,
        };
        return colors;
    }

    struct ColorTxt {
        static inline std::string Colorize(const std::string& str, Colors c) {
            return _FMT::format("\x1b[1;{}m{}{}", static_cast<int>(c), str, Reset());
        }
        static inline std::string Reset() { return "\x1b[0m"; }
    };
};  // namespace cliColors