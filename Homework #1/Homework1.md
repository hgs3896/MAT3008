# Machine Epsilon

## 1. Method 1: using 'machar' function in NA library

By calling `machar` with proper argument values, the machine epsilon can be calculated.  
In order to exploit the C++ template function, redefine the `machar` as a template function.

```c++
template<typename DataType>
static void machar(int *ibeta, int *it, int *irnd, int *ngrd, int *machep, int *negep,
	int *iexp, int *minexp, int *maxexp, DataType *eps, DataType *epsneg,
	DataType *xmin, DataType *xmax)
```

## 2. Method 2: calculating eps by finding the largest positive integer n that satisfies 1 + 2^(-n) != 1

Finding the largest positive integer **n** that satisfies 1 + 2^(-n) != 1, it matches with the definition of the machine epsilon.
Also, using C++ template feature, it is required to create only single implementation of `get_eps()`.

```c++
template <typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type get_eps(){
    const T one = T(1);
    T eps = one;
    while(one + eps / 2 != one)
        eps /= 2;
    return eps;
}
```