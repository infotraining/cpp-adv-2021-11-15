#include <algorithm>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;

bool cbl(const std::string& s1, const std::string& s2)
{
    return s1.length() < s2.length();
}

class Lambda_482736482734
{
public:
    auto operator()() const { std::cout << "Hello from lambda!\n"; }
};

TEST_CASE("lambda expressions")
{
    auto closure = []()
    { std::cout << "Hello from lambda!\n"; };
    closure();

    SECTION("is interpreted as")
    {
        auto closure = Lambda_482736482734 {};
        closure();
    }

    auto add = [](int a, int b)
    { return a + b; };

    REQUIRE(add(1, 2) == 3);
}

class Lambda_784365238743
{
    const int ratio_;

public:
    Lambda_784365238743(int ratio)
        : ratio_ {ratio}
    {
    }

    auto operator()(int x) const { return x * ratio_; }
};

TEST_CASE("full closures")
{
    SECTION("capture by value")
    {
        int ratio = 10;

        auto scaler = [ratio](int x)
        { return x * ratio; };

        REQUIRE(scaler(5) == 50);

        ratio = 20;

        REQUIRE(scaler(5) == 50);
    }

    SECTION("capture by ref")
    {
        int sum {};

        auto sum_up = [&sum](int value)
        { sum += value; };

        std::vector<int> data = {1, 2, 3, 4, 5};

        std::for_each(begin(data), end(data), sum_up);

        REQUIRE(sum == 15);
    }

    SECTION("capture by ref and by value")
    {
        int sum {};
        int ratio = 10;

        auto sum_up = [&sum, ratio](int value)
        { sum += value * ratio; };

        std::vector<int> data = {1, 2, 3, 4, 5};

        std::for_each(begin(data), end(data), sum_up);

        REQUIRE(sum == 150);
    }

    SECTION("closures are immutable by default")
    {
        int gen_id {};

        auto get_next_id = [gen_id] () mutable { return ++gen_id; };

        REQUIRE(get_next_id() == 1);
        REQUIRE(get_next_id() == 2);
        REQUIRE(get_next_id() == 3);
    }
}

auto create_genereator(int seed)
{
    return [seed]() mutable { return ++seed; };
}

TEST_CASE("lambda returned from function")
{
    auto gen = create_genereator(1000);

    REQUIRE(gen() == 1001);
    REQUIRE(gen() == 1002);
}


TEST_CASE("using lambdas")
{
    SECTION("std algorithm")
    {
        std::vector<std::string> words = { "one", "two", "seven", "nine", "one hundred"};

        std::sort(begin(words), end(words), [](const auto& s1, const auto& s2) { return s1.length() < s2.length(); });

        char c = 'n';
        auto pos = std::find_if(begin(words), end(words), [c](const auto& s) { return s[0] == c; });
    }
}


TEST_CASE("how to store lambda")
{
    SECTION("auto - the best option")
    {
        int sum {};
        int ratio = 10;

        auto sum_up = [&sum, ratio](int value) { sum += value * ratio; };

        sum_up(20);
    }

    SECTION("lambda with empty []")
    {
        int (*ptr_fun)(int, int);

        ptr_fun = [](int a, int b) { return a + b; };
    }

    SECTION("std::function")
    {
        int sum {};
        int ratio = 10;

        std::function<void(int)> sum_up = [&sum, ratio](int value) { sum += value * ratio; };

        sum_up = [](int x) { return x + 2; }; // std::function is typed erased

        sum_up(20);
    }
}


template <typename TCallable>
void call(TCallable c)
{
    c();
}

namespace WorseOption
{
    void call(std::function<void()> f)
    {
        f();
    }
}

TEST_CASE("pass lambda to function")
{
    auto print = [] { std::cout << "LAMBDA!\n"; };

    call(print);
    call([] { std::cout << "LAMBDA2!\n"; });

    WorseOption::call(print);
    WorseOption::call([] { std::cout << "LAMBDA3!\n"; });
}