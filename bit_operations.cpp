#include <bitset>
#include <iostream>

#define PRINT_BITS(x)                                  \
    std::cout << #x << '\t' << x << '\t'               \
              << std::bitset<sizeof(x) * 8>{x} << '\t' \
              << std::hex << x << std::dec << '\n';

int main() {
    const uint32_t first{0x1337}, second{0xC0FFEE}, third{0xDEAD};

    PRINT_BITS(first);
    PRINT_BITS(second);
    PRINT_BITS(third);

    PRINT_BITS((first | second));
    PRINT_BITS((first & second));
    PRINT_BITS((first ^ second));
    PRINT_BITS((first & third));
    PRINT_BITS((third ^ second));
    PRINT_BITS((first | third));
    PRINT_BITS((third | second));
}