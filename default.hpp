#pragma once
#include <any>
#include <array>
#include <bitset>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>
#include <vector>

#define PRINT(x) \
    std::cout << #x << '\t' << x << '\n';

// TODO FIX
#define PRINT_BITS(x) \
    std::cout << #x << '\t' << x << '\t' << std::bitset<sizeof(x) * 8>{x} << '\t' << std::hex << x << std::dec << '\n';
