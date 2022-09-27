#include <iostream>
#include <iomanip>
#include <string>

using std::string_literals::operator""s;

template <typename T> constexpr
void print_value(const std::string& label, T value){
    std::cout << std::fixed;
    std::cout << label << " = ";
    if constexpr (std::is_floating_point_v<T>)
        std::cout << std::setprecision(std::numeric_limits<T>::digits + 1);
    std::cout << value << std::endl;
}

template<typename T> constexpr
T f(T x){
    return T(6) * x / (T(1) - T(3) * x * x) / (T(1) - T(3) * x * x);
}

int main(int argc, char const *argv[])
{
    print_value("f'(x = 0.577)"s, f<float>(0.577));
    print_value("f'(x = 0.577)"s, "0.0067379470000000000000000");

    print_value("f'(x = 0.577)"s, f<double>(0.577));
    print_value("f'(x = 0.577)"s, "0.006737947000000000000000000000000000000000000000000000");
    return 0;
}
