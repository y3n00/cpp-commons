#pragma once
#include <fmt/core.h>

#include <functional>
#include <string_view>
#include <vector>

#define CONCAT(a, b) a##b
#define PAD_NAME(n) CONCAT(pad, n)
#define PAD(size) std::byte PAD_NAME(__LINE__)[size];

/*
TODO to fix memory::findString
*/

namespace memory {
template <typename ptrT>
struct SearchIn {
    SearchIn<ptrT>(ptrT* ptr, size_t size)
        : _base{ptr}, _rangeSize{size} {}

    SearchIn<ptrT>(ptrT* ptr)
        : _base{ptr}, _rangeSize{sizeof(ptrT)} {}

    ptrT* _base = nullptr;
    size_t _rangeSize = 0;
};

template <typename ptrT>
struct Result {
    ptrT* _ptr = nullptr;
    size_t _offset = 0;

    [[nodiscard]] ptrT
    getValue() const {
        return *_ptr;
    }
};

template <typename ptrT, typename PrintType>
void printAs(SearchIn<ptrT> si) {
    const size_t size = si._rangeSize / sizeof(PrintType);

    PrintType* castedRange = reinterpret_cast<PrintType*>(si._base);

    for (size_t idx = 0; idx < si._rangeSize; idx++) {
        fmt::print("{:#x}\t", castedRange[idx]);
        if (idx % 16 == 0 && idx != 0)
            fmt::print("\n");
    }
    fmt::print("\n\n");
}

template <typename ptrT, typename TypeToSearch>
[[nodiscard]] Result<TypeToSearch>
find(SearchIn<ptrT> si, TypeToSearch value) {
    const size_t size = si._rangeSize / sizeof(TypeToSearch);
    TypeToSearch* castedRange = reinterpret_cast<TypeToSearch*>(si._base);

    Result<TypeToSearch> r;
    for (size_t idx = 0; idx < size; idx++) {
        if (castedRange[idx] == value) {
            r = {&castedRange[idx], idx};
            break;
        }
    }
    return r;
}

template <typename ptrT, typename TypeToSearch>
[[nodiscard]] std::vector<Result<TypeToSearch>>
findAll(SearchIn<ptrT> si, TypeToSearch value) {
    std::vector<Result<TypeToSearch>> pointers;
    const size_t size = si._rangeSize / sizeof(TypeToSearch);
    TypeToSearch* castedRange = reinterpret_cast<TypeToSearch*>(si._base);

    for (size_t idx = 0; idx < size; idx++)
        if (castedRange[idx] == value)
            pointers.push_back({&castedRange[idx], idx});

    return pointers;
}

//! DOESNT WORK
template <typename ptrT>
[[nodiscard]] Result<char*>
findString(SearchIn<ptrT> si, std::string value) {
    const size_t size = si._rangeSize;
    auto* castedRange = reinterpret_cast<char*>(si._base);

    Result<char*> r;
    for (size_t idx = 0; idx < size; idx++) {
        char* charPtr = (castedRange + idx);
        if (std::string_view{charPtr, value.size()} == value) {
            std::cout << "find at " << idx << '\n';
            r = {&charPtr, idx};
            break;
        }
    }
    return r;
}

template <typename ptrT, typename TypeToSearch>
[[nodiscard]] TypeToSearch
findValueWithOffset(ptrT* base, size_t offset) {
    offset /= sizeof(TypeToSearch);
    auto ptr = reinterpret_cast<TypeToSearch*>(base);
    return ptr[offset];
}

template <typename T, typename U>
[[nodiscard]] ptrdiff_t
getOffset(T* t, U* u) {
    return reinterpret_cast<std::byte*>(u) - reinterpret_cast<std::byte*>(t);
}

template <typename ptrT, typename TypeToSearch>
class dynamicScan {
   public:
    dynamicScan(SearchIn<ptrT> si, TypeToSearch value)
        : _v{value}, _si{si} {
        const size_t size = _si._rangeSize / sizeof(TypeToSearch);
        TypeToSearch* castedRange = reinterpret_cast<TypeToSearch*>(_si._base);

        for (size_t idx = 0; idx < size; idx++)
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
            std::cout << "base [" << _si._base << "] +\n";
            for (auto& v : _results) {
                const ptrdiff_t offset = getOffset(_si._base, v._ptr);
                std::cout << "\t\t\t" << offset << " = " << v.getValue() << '\n';
            }
        }
    }

   private:
    TypeToSearch _v;
    SearchIn<ptrT> _si;
    std::vector<Result<TypeToSearch>> _results;
};

}  // namespace memory
