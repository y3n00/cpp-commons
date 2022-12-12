#include "default.hpp"

inline static void printAny(const std::any& any) {
#define PRINT_AS(T)                   \
    if (any.type() == typeid(T))      \
        PRINT(std::any_cast<T>(any)); \
    return;

    PRINT_AS(char);
    PRINT_AS(float);
    PRINT_AS(double);
    PRINT_AS(int32_t);
    PRINT_AS(int64_t);
    PRINT_AS(uint64_t);
    PRINT_AS(std::string);

#undef PRINT_AS
}

int main() {
    static const std::array<std::any, 6> any_arr{
        std::string("jhasdhjkashjk"),
        '&',
        3.1415926535,
        0.25f,
        125,
        std::numeric_limits<uint64_t>::max(),
    };

    for (auto&& elem : any_arr)
        printAny(elem);
}

/* OUTPUT
std::any_cast<std::string>(any) jhasdhjkashjk
std::any_cast<char>(any)        &
std::any_cast<double>(any)      3.14159
std::any_cast<float>(any)       0.25
std::any_cast<int32_t>(any)     125
std::any_cast<uint64_t>(any)    18446744073709551615
*/