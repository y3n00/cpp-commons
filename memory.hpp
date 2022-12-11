#include <fmt/core.h>

#include <string_view>

namespace memory {
template <typename T, typename U>
void printAs(T* t) {
    constexpr size_t size = sizeof(T) / sizeof(U);
    U* u = reinterpret_cast<U*>(t);

    for (auto idx = 0; idx < size; idx++) {
        fmt::print("{:#x}\t", u[idx]);
        if (idx % 16 == 0 && idx != 0)
            std::cout << '\n';
    }
    fmt::print("\n\n");
}

template <typename T, typename U>
U* findOffset(T* where, U value) {
    constexpr size_t size = sizeof(T) / sizeof(U);
    U* u = reinterpret_cast<U*>(where);
    U* retPtr;
    for (auto idx = 0; idx < size; idx++) {
        if (u[idx] == value) {
            retPtr = (u + idx);
            break;
        }
    }
    return retPtr;
}

template <typename U>
U* findOffset(std::byte* searchFrom, U value, size_t sizeInBytes) {
    const size_t size = sizeInBytes / sizeof(U);
    U* u = reinterpret_cast<U*>(searchFrom);
    U* retPtr;
    for (auto idx = 0; idx < size; idx++) {
        if (u[idx] == value) {
            retPtr = (u + idx);
            break;
        }
    }
    return retPtr;
}

template <typename T>
auto* findOffset(T* where, std::string_view value) {
    constexpr size_t size = value.size();
    char* u = reinterpret_cast<char*>(where);
    char* retPtr;

    for (auto idx = 0; idx < size; idx++) {
        if (std::string_view{u[idx], size} == value) {
            retPtr = (u + idx);
            break;
        }
    }
    return retPtr;
}

template <typename T, typename U>
ptrdiff_t getDiff(T* t, U* u) {
    std::byte* v1 = reinterpret_cast<std::byte*>(t);
    std::byte* v2 = reinterpret_cast<std::byte*>(u);

    return ptrdiff_t(v2 - v1);
}
}  // namespace memory