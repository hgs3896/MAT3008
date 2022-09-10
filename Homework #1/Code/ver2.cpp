#include <type_traits>
#include <iostream>
#include <limits>
#include <iomanip>
#include <cmath>
#include <cassert>

template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type get_eps(){
    const T one = T(1);
    T eps = one;
    while(one + eps / 2 != one)
        eps /= 2;
    return eps;
}

template <typename T>
void print_machine_eps(){
    std::cout << std::fixed;
    std::cout << "machep = " << std::setprecision(std::numeric_limits<T>::digits + 1);
    std::cout << get_eps<T>() << std::endl;
}

int main(){
    print_machine_eps<float>();
    print_machine_eps<double>();
    return 0;
}