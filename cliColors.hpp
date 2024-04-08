#pragma once

#if defined(__cplusplus) && (__cplusplus >= 202002L)
#include <format>
namespace _FMT = std;
#else
#include <fmt/format-inl.h>
namespace _FMT = fmt;
#endif

#include <string>

namespace cliColors {
    enum class Colors {
        red,
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
    };

    struct ColorTxt {
        static inline std::string Colorize(const std::string& str, Colors c) {
            return _FMT::format("\x1b[1;{}m{}{}", (static_cast<int>(c) + 30), str, Reset());
        }

        static inline std::string Reset() { return "\x1b[0m"; }
    };
};  // namespace cliColors