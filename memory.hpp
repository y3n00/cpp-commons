#pragma once
#include <fmt/core.h>

#include <functional>
#include <string_view>
#include <vector>

#define CONCAT(a, b) a##b
#define PAD_NAME(n) CONCAT(pad, n)
#define PAD(size) std::byte PAD_NAME(__LINE__)[size];

namespace memory {
template <typename T>
struct SearchIn {
    T* _base;
    size_t _blockSize;
};

template <typename T, typename U>
void printAs(T* base) {
    constexpr size_t size = sizeof(T) / sizeof(U);
    U* vts = reinterpret_cast<U*>(base);

    for (auto idx = 0; idx < size; idx++) {
        fmt::print("{:#x}\t", vts[idx]);
        if (idx % 16 == 0 && idx != 0)
            fmt::print("\n");
    }
    fmt::print("\n\n");
}

template <typename T, typename U>
[[nodiscard]] U* findAddress(T* base, U valueToSearch) {
    constexpr size_t size = sizeof(T) / sizeof(U);
    U* vts = reinterpret_cast<U*>(base);

    U* retPtr;
    for (auto idx = 0; idx < size; idx++) {
        if (vts[idx] == valueToSearch) {
            retPtr = (vts + idx);
            break;
        }
    }
    return retPtr;
}

template <typename T, typename U>
[[nodiscard]] U* findAddress(SearchIn<T> si, U valueToSearch) {
    const size_t size = si._blockSize / sizeof(U);
    U* vts = reinterpret_cast<U*>(si._base);

    U* retPtr;
    for (auto idx = 0; idx < size; idx++) {
        if (vts[idx] == valueToSearch) {
            retPtr = (vts + idx);
            break;
        }
    }
    return retPtr;
}

template <typename T, typename U>
[[nodiscard]] auto findAll(SearchIn<T> si, U valueToSearch) {
    std::vector<U*> pointers;
    const size_t size = si._blockSize / sizeof(U);
    U* vts = reinterpret_cast<U*>(si._base);

    for (auto idx = 0; idx < size; idx++)
        if (vts[idx] == valueToSearch)
            pointers.push_back(vts + idx);

    return pointers;
}

template <typename T>
[[nodiscard]] auto* findAddress(T* base, std::string_view valueToSearch) {
    constexpr size_t size = valueToSearch.size();
    char* vts = reinterpret_cast<char*>(base);
    char* retPtr;

    for (auto idx = 0; idx < size; idx++) {
        if (std::string_view{vts[idx], size} == valueToSearch) {
            retPtr = (vts + idx);
            break;
        }
    }
    return retPtr;
}

template <typename T, typename U>
U findValueWithOffset(T* base, size_t offset) {
    offset /= sizeof(U);
    auto ptr = reinterpret_cast<U*>(base);
    return *(ptr + offset);
}

template <typename T, typename U>
[[nodiscard]] ptrdiff_t getOffset(T* t, U* u) {
    return ptrdiff_t(
        reinterpret_cast<uint8_t*>(u) - reinterpret_cast<uint8_t*>(t));
}

template <typename T, typename U>
class dynamicScan {
   public:
    dynamicScan(SearchIn<T> si, U valueToSearch)
        : _si{si}, _v{valueToSearch} {
        const size_t size = _si._blockSize / sizeof(U);
        U* vts = reinterpret_cast<U*>(_si._base);

        for (auto idx = 0; idx < size; idx++)
            if (vts[idx] == _v)
                _pointers.push_back(vts + idx);
    }

    void nextScan(std::function<bool(U, U)> compare) {
        std::vector<U*> newVec;

        for (auto& p : _pointers)
            if (compare(*p, _v))
                newVec.push_back(p);

        newVec.swap(_pointers);
    }

    void nextScan(U newValue) {
        std::vector<U*> newVec;
        _v = newValue;

        for (auto& p : _pointers)
            if (*p == _v)
                newVec.push_back(p);

        newVec.swap(_pointers);
    }

    [[nodiscard]] auto getResultVec() {
        return _pointers;
    }

    [[nodiscard]] auto getOffsetsVec() {
        std::vector<size_t> offsets;
        for (auto& v : _pointers)
            offsets.emplace_back(std::move(getOffset(_si._base, v)));

        return offsets;
    }

    void printResults() {
        for (const auto& v : _pointers)
            std::cout << "base(" << _si._base << ") + " << getOffset(_si._base, v) << " = " << *v << '\n';
    }

   private:
    std::vector<U*> _pointers;
    SearchIn<T> _si;
    U _v;
};

}  // namespace memory