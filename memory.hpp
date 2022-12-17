#pragma once
#include <fmt/core.h>

#include <functional>
#include <string_view>
#include <vector>

#define CONCAT(a, b) a##b
#define PAD_NAME(n) CONCAT(pad, n)
#define PAD(size) std::byte PAD_NAME(__LINE__)[size];

namespace memory {
template <typename ptrT>
struct SearchIn {
    ptrT* _base;
    size_t _blockSize;
};

template <typename ptrT>
struct Result {
    ptrT* _ptr;
    ptrdiff_t _offset;
    auto getValue() const {
        return *_ptr;
    }
};

template <typename ptrT, typename SearchType>
void printAs(SearchIn<ptrT> si) {
    const size_t size = si._blockSize / sizeof(SearchType);
    SearchType* vts = reinterpret_cast<SearchType*>(si._base);

    for (auto idx = 0; idx < si._blockSize; idx++) {
        fmt::print("{:#x}\t", vts[idx]);
        if (idx % 16 == 0 && idx != 0)
            fmt::print("\n");
    }
    fmt::print("\n\n");
}

template <typename ptrT, typename SearchType>
[[nodiscard]] Result<SearchType> find(SearchIn<ptrT> si, SearchType value) {
    const size_t size = si._blockSize / sizeof(SearchType);
    SearchType* vts = reinterpret_cast<SearchType*>(si._base);

    Result<SearchType> r;
    for (auto idx = 0; idx < size; idx++) {
        if (vts[idx] == value) {
            r = {&vts[idx], idx};
            break;
        }
    }
    return r;
}

template <typename ptrT, typename SearchType>
[[nodiscard]] auto findAll(SearchIn<ptrT> si, SearchType value) {
    std::vector<Result<SearchType>> pointers;
    const size_t size = si._blockSize / sizeof(SearchType);
    SearchType* vts = reinterpret_cast<SearchType*>(si._base);

    for (auto idx = 0; idx < size; idx++)
        if (vts[idx] == value)
            pointers.push_back({&vts[idx], idx});

    return pointers;
}

template <typename ptrT>
[[nodiscard]] auto* find(SearchIn<ptrT> si, std::string_view value) {
    constexpr size_t size = value.size();
    char* vts = reinterpret_cast<char*>(si._base);
    Result<char*> r;
    char* retPtr;

    for (auto idx = 0; idx < size; idx++) {
        if (std::string_view{vts[idx], size} == value) {
            r = {&vts[idx], idx};
            break;
        }
    }
    return r;
}

template <typename ptrT, typename SearchType>
SearchType findValueWithOffset(ptrT* base, size_t offset) {
    offset /= sizeof(SearchType);
    auto ptr = reinterpret_cast<SearchType*>(base);
    return ptr[offset];
}

template <typename ptrT, typename SearchType>
[[nodiscard]] ptrdiff_t getOffset(ptrT* t, SearchType* u) {
    return ptrdiff_t(
        reinterpret_cast<uint8_t*>(u) - reinterpret_cast<uint8_t*>(t));
}

template <typename ptrT, typename SearchType>
class dynamicScan {
   public:
    dynamicScan(SearchIn<ptrT> si, SearchType value)
        : _v{value}, _si{si} {
        const size_t size = _si._blockSize / sizeof(SearchType);
        SearchType* vts = reinterpret_cast<SearchType*>(_si._base);

        for (auto idx = 0; idx < size; idx++)
            if (vts[idx] == _v)
                _results.push_back({vts + idx, idx});
    }

    void nextScan(std::function<bool(SearchType, SearchType)> compare) {
        decltype(_results) newVec;

        for (auto& p : _results) {
            if (compare(p.getValue(), _v))
                newVec.push_back(p);
        }
        newVec.swap(_results);
    }

    void nextScan(SearchType newValue) {
        decltype(_results) newVec;
        _v = newValue;

        for (auto& p : _results)
            if (p.getValue() == _v)
                newVec.push_back(p);

        newVec.swap(_results);
    }

    [[nodiscard]] auto getResultVec() {
        return _results;
    }

    void printResults() {
        for (auto& v : _results)
            std::cout << "base(" << _si._base << ") + " << getOffset(_si._base, v._ptr) << " = " << v.getValue() << '\n';
    }

   private:
    SearchType _v;
    SearchIn<ptrT> _si;
    std::vector<Result<SearchType>> _results;
};

}  // namespace memory