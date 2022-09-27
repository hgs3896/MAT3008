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
T e_pow_x(int x, int terms_to_use){
    T result = 1;
    T powed = 1;
    T fact = 1;
    for(int k = 1; k < terms_to_use; ++k){
        powed *= x; // powed = x^k
        fact *= k;  // fact  = k!
        result += powed / fact;
    }
    return result;
}

int main(int argc, char const *argv[])
{
    print_value("e^(-5)"s, e_pow_x<float>(-5, 20));
    print_value("e^(-5)"s, 1.f / e_pow_x<float>(5, 20));
    print_value("e^(-5)"s, "0.0067379470000000000000000");

    print_value("e^(-5)"s, e_pow_x<double>(-5, 20));
    print_value("e^(-5)"s, 1.f / e_pow_x<double>(5, 20));
    print_value("e^(-5)"s, "0.006737947000000000000000000000000000000000000000000000");
    return 0;
}
