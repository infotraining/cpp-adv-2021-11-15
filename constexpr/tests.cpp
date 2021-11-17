#include "catch.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <array>

using namespace std;
using namespace Catch::Matchers;

constexpr int factorial(int n)
{
    return (n == 0) ? 1 : n * factorial(n-1);
}

template <size_t N>
constexpr std::array<int, N> create_factorial_lookup()
{
    std::array<int, N> result{};
 
    for(int n = 0; n < N; ++n)
        result[n] = factorial(n);

    return result;
}

TEST_CASE("constexpr")
{
    constexpr double pi = 3.14;
    constexpr double dpi = pi * 2.0;

    int tab[factorial(4)] = {};

    std::array<int, factorial(4)> arr = {};
    static_assert(arr.size() == 24);

    constexpr auto factorial_lookup = create_factorial_lookup<10>();

    SECTION("constexpr lambda - since C++17")
    {
        auto square = [](int x) { return x * x; };

        std::array<int, square(10)> arr = {};
        static_assert(arr.size() == 100);
    }
}

