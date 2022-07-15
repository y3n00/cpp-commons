#include <any>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#define print(x) \
    std::cout << #x << '\t' << x << '\n';

inline static void printAny(const std::any& any) {
#define PRINT_IF(T)              \
    if (any.type() == typeid(T)) \
        print(std::any_cast<T>(any));

    PRINT_IF(char);
    PRINT_IF(float);
    PRINT_IF(double);
    PRINT_IF(int32_t);
    PRINT_IF(int64_t);
    PRINT_IF(uint64_t);
    PRINT_IF(std::string);

#undef PRINT_IF
}

int main() {
    const static std::vector<std::any> any_vec{
        std::string("jhasdhjkashjk"),
        '&',
        3.1415926535,
        0.25f,
        125,
        std::numeric_limits<uint64_t>::max(),
    };

    for (auto&& elem : any_vec)
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