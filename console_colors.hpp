#pragma once

#if defined(_MSVC_LANG)
#define CPP_STANDARD _MSVC_LANG
#else
#define CPP_STANDARD __cplusplus
#endif

#if (CPP_STANDARD >= 202002L)
#include <format>
#define _FMT std
#else
#include <fmt/format.h>
#define _FMT fmt
#endif

#include <array>
#include <string>

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

    [[nodiscard]] constexpr auto getAllColors() {
        constexpr auto colors = std::to_array({
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
        });
        return colors;
    }

    namespace ColorTxt {
        [[nodiscard]] inline std::string Reset() { return "\x1b[0m"; }

        [[nodiscard]] std::string Colorize(const std::string& str, Colors c) {
            return _FMT::format("\x1b[1;{}m{}{}", static_cast<int>(c), str, Reset());
        }
    }  // namespace ColorTxt
};  // namespace cliColors

#undef _FMT
#undef CPP_STANDARD