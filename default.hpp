#pragma once
#include <any>
#include <array>
#include <bitset>
#include <chrono>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "Random.hpp"
#include "Timer.hpp"

#define PRINT(x) \
    std::cout << #x << '\t' << x << '\n';

// TODO FIX
#define PRINT_BITS(x)                                  \
    std::cout << #x << '\t' << x << '\t'               \
              << std::bitset<sizeof(x) * 8>{x} << '\t' \
              << std::hex << x << std::dec << '\n';