#pragma once
#include <any>
#include <array>
#include <bitset>
#include <chrono>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "Random.hpp"
#include "Timer.hpp"

#define PRINT(x) \
    std::cout << #x << '\t' << x << '\n';