#include "default.hpp"

template <typename T>
class TestInitList {
   private:
    std::unique_ptr<T[]> T_ptr;
    size_t size;

   public:
    TestInitList() = delete;
    TestInitList(TestInitList&&) = delete;
    TestInitList(const TestInitList&) = delete;

    TestInitList(std::initializer_list<T>&& il) noexcept
        : T_ptr{new T[il.size()]},
          size{il.size()} {
        size_t counter{};
        for (auto iter = il.begin(); iter != il.end(); ++iter)
            T_ptr[counter++] = std::move(*iter);
    }

    friend void Print(const TestInitList<T>& TIL) {
        for (size_t idx{}; idx < TIL.size; ++idx)
            std::cout << TIL.T_ptr[idx] << '\n';
    }

    ~TestInitList() noexcept = default;
};

int main() {
    struct SSS {
        SSS() { std::puts("ctor"); }
        ~SSS() noexcept { std::puts("dector"); }
    };
    TestInitList<int> t_int{10, 20, 40, 50};
    TestInitList<std::string> t_str{"sadadsa", "sadasdasd"};
    TestInitList<SSS> t_sss{{}, {}, {}};

    Print(t_int);
    Print(t_str);
}

/* OUTPUT
10
20
40
50
sadadsa
sadasdasd
*/