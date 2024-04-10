#pragma once

#include <cstdint>
#include <string>
#include <vector>

using u8 = std::uint_least8_t;
using i8 = std::int_least8_t;
using u16 = std::uint_least16_t;
using i16 = std::int_least16_t;
using u32 = std::uint_least32_t;
using i32 = std::int_least32_t;
using u64 = std::uint_least64_t;
using i64 = std::int_least64_t;
using f32 = float;
using f64 = double;

namespace std {
    using str = string;
    using str_v = string_view;

    template <typename T>
    using vec = vector;
}  // namespace std