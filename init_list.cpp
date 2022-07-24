#include "default.hpp"

template <typename T>
class Test {
   private:
    T* T_ptr;
    size_t size;

   public:
    Test() = delete;
    Test(Test&) = delete;
    Test(Test&&) = delete;

    Test(std::initializer_list<T> il)
        : T_ptr{new T[il.size()]},
          size{il.size()} {
        size_t counter{};
        for (auto iter = il.begin(); iter != il.end(); ++iter)
            T_ptr[counter++] = *iter;
    }

    void Print() const {
        for (size_t idx{}; idx < size; ++idx)
            std::cout << T_ptr[idx] << '\n';
    }

    ~Test() noexcept { delete[] T_ptr; }
};

int main() {
    Test<int> t_int{10, 20, 40, 50};
    t_int.Print();
    Test<std::string> t_str{"sadadsa", "sadasdasd"};
    t_str.Print();
}

/* OUTPUT
10
20
40
50
sadadsa
sadasdasd
*/