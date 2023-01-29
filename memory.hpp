#pragma once
#include <functional>
#include <iomanip>
#include <iostream>
#include <span>
#include <string_view>
#include <vector>

#define HEXED(x) \
    std::hex << std::showbase << x << std::noshowbase << std::dec

#define CONCAT(a, b) a##b
#define PAD_NAME(n) CONCAT(pad, n)
#define PAD(size) std::byte PAD_NAME(__LINE__)[size];

/*
TODO    to fix memory::findString
TODO    raplace SearchIn by std::span
*/

namespace memory {
template <typename ptrT>
struct Result {
    ptrT* _ptr = nullptr;
    size_t _offset = 0;

    [[nodiscard]] ptrT
    getValue() const {
        return *_ptr;
    }
};

template <typename PrintType>
void printAs(std::span<std::byte> bytes) {
    PrintType* castedRange = reinterpret_cast<PrintType*>(bytes);

    for (size_t idx = 0; idx < si._rangeSize; idx++) {
        std::cout << HEXED(castedRange[idx]) << '\t';
        if (idx % 16 == 0 && idx != 0)
            std::cout << '\n';
    }
    std::cout << "\n\n";
}

template <typename TypeToSearch>
[[nodiscard]] Result<TypeToSearch>
find(std::span<std::byte> bytes, TypeToSearch value) {
    TypeToSearch* castedRange = reinterpret_cast<TypeToSearch*>(bytes);

    Result<TypeToSearch> r;
    for (size_t idx = 0; idx < size; idx++) {
        if (castedRange[idx] == value) {
            r = {&castedRange[idx], idx};
            break;
        }
    }
    return r;
}

template <typename TypeToSearch>
[[nodiscard]] std::vector<Result<TypeToSearch>>
findAll(std::span<std::byte> bytes, TypeToSearch value) {
    std::vector<Result<TypeToSearch>> pointers;
    TypeToSearch* castedRange = reinterpret_cast<TypeToSearch*>(bytes);

    for (size_t idx = 0; idx < size; idx++)
        if (castedRange[idx] == value)
            pointers.push_back({&castedRange[idx], idx});

    return pointers;
}

//! DOESNT WORK
template <typename ptrT>
[[nodiscard]] Result<char*>
findString(std::span<std::byte> bytes, std::string value) {
    auto* castedRange = reinterpret_cast<char*>(bytes);

    Result<char*> r;
    for (size_t idx = 0; idx < bytes.size(); idx++) {
        char* charPtr = (castedRange + idx);
        if (std::string_view{charPtr, value.size()} == value) {
            std::cout << "find at " << idx << '\n';
            r = {&charPtr, idx};
            break;
        }
    }
    return r;
}

template <typename TypeToSearch>
[[nodiscard]] TypeToSearch
findValueWithOffset(std::span<std::byte> base, size_t offset) {
    offset /= sizeof(TypeToSearch);
    auto ptr = reinterpret_cast<TypeToSearch*>(base);
    return ptr[offset];
}

template <typename T, typename U>
[[nodiscard]] ptrdiff_t
getOffset(T* t, U* u) {
    return reinterpret_cast<std::byte*>(u) - reinterpret_cast<std::byte*>(t);
}

template <typename TypeToSearch>
class dynamicScan {
   public:
    dynamicScan(std::span<std::byte> bytes, TypeToSearch value)
        : _v{value}, memPool{bytes} {
        TypeToSearch* castedRange = reinterpret_cast<TypeToSearch*>(bytes);
        memPool.size();
        for (size_t idx = 0; idx < ; idx++)
            if (castedRange[idx] == _v)
                _results.push_back({castedRange + idx, idx});
    }

    void nextScan(std::function<bool(TypeToSearch, TypeToSearch)> compare) {
        decltype(_results) newVec;

        for (auto& p : _results) {
            if (compare(p.getValue(), _v))
                newVec.push_back(p);
        }
        newVec.swap(_results);
    }

    void nextScan(TypeToSearch newValue) {
        decltype(_results) newVec;
        _v = newValue;

        for (auto& p : _results)
            if (p.getValue() == _v)
                newVec.push_back(p);

        newVec.swap(_results);
    }

    [[nodiscard]] std::vector<Result<TypeToSearch>>
    getResultVec() {
        return _results;
    }

    void printResults() {
        if (_results.size()) {
            std::cout << "base [" << _bytes << "] +\n";
            for (auto& v : _results) {
                const ptrdiff_t offset = getOffset(_bytes, v._ptr);
                std::cout << "\t\t\t" << offset << " = " << v.getValue() << '\n';
            }
        }
    }

   private:
    TypeToSearch _v;
    std::span<std::byte> memPool;
    std::vector<Result<TypeToSearch>> _results;
};

}  // namespace memory

#undef HEXED
