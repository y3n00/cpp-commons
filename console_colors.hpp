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
        blue = 34,
        purple = 35,
        cyan = 36,
        gray = 37,
        brightgray = 90,
        brightred = 91,
        brightgreen = 92,
        lightblue = 94,
        magenta = 95,
        lightcyan = 96,
        white = 97,
        _default = 39,
    };

    Colors& operator++(Colors& color) {
        switch (color) {
            case Colors::reset:
                color = Colors::black;
                break;
            case Colors::black:
                color = Colors::red;
                break;
            case Colors::red:
                color = Colors::green;
                break;
            case Colors::green:
                color = Colors::yellow;
                break;
            case Colors::yellow:
                color = Colors::blue;
                break;
            case Colors::blue:
                color = Colors::purple;
                break;
            case Colors::purple:
                color = Colors::cyan;
                break;
            case Colors::cyan:
                color = Colors::gray;
                break;
            case Colors::gray:
                color = Colors::brightgray;
                break;
            case Colors::brightgray:
                color = Colors::brightred;
                break;
            case Colors::brightred:
                color = Colors::brightgreen;
                break;
            case Colors::brightgreen:
                color = Colors::lightblue;
                break;
            case Colors::lightblue:
                color = Colors::magenta;
                break;
            case Colors::magenta:
                color = Colors::lightcyan;
                break;
            case Colors::lightcyan:
                color = Colors::white;
                break;
            case Colors::white:
                color = Colors::_default;
                break;
            case Colors::_default:
                color = Colors::reset;
                break;
        }
        return color;
    }

    Colors operator++(Colors& color, int) {
        Colors temp = color;
        ++color;
        return temp;
    }

    [[nodiscard]] constexpr auto getAllColors() {
        constexpr auto colors = std::to_array({
            Colors::reset,
            Colors::black,
            Colors::red,
            Colors::green,
            Colors::yellow,
            Colors::blue,
            Colors::purple,
            Colors::cyan,
            Colors::gray,
            Colors::brightgray,
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
        [[nodiscard]] inline constexpr std::string_view Reset() noexcept {
            return "\x1b[0m";
        }

        [[nodiscard]] inline std::string Colorize(std::string_view str, Colors c) {
            return _FMT::format("\x1b[1;{}m{}{}", std::to_underlying(c), str, Reset());
        }
    }  // namespace ColorTxt
};  // namespace cliColors

#undef _FMT
#undef CPP_STANDARD