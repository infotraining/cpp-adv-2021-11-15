#include "catch.hpp"
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <list>

using namespace std;

int add_classic(int a, int b)
{
    return a + b;
}

auto add_cpp11(int a, int b) -> int
{
    return a + b;
}

template <typename T>
auto multiply_generic(const T& a, const T& b)
{
    return a * b;
}

class Lambda_452376547625
{
public:
    template <typename T1, typename T2>
    auto operator()(const T1& a, const T2& b) const
    {
        return *a < *b;
    }
};

auto lambda_with_arrow = [](int a) -> std::string
{
    if (a % 2 == 0)
        return "even";
    else
        return "odd"s;
};

auto lambda_add = [](auto a, auto b) -> std::common_type_t<decltype(a), decltype(b)>
{
    return a + b;
};

TEST_CASE("generic lambda")
{
    auto cmp_by_pointed_value = [](const auto& a, const auto& b)
    { return *a < *b; };

    std::set<std::unique_ptr<int>, decltype(cmp_by_pointed_value)> values(cmp_by_pointed_value);
    values.insert(std::make_unique<int>(5));
    values.insert(std::make_unique<int>(1));
    values.insert(std::make_unique<int>(6));
    values.insert(std::make_unique<int>(7));
    values.insert(std::make_unique<int>(2));

    for (const auto& v : values)
    {
        std::cout << *v << " ";
    }
    std::cout << "\n";
}

template <typename T>
T maximum(T a, T b)
{
    puts(__PRETTY_FUNCTION__);
    return a < b ? b : a;
}

template <typename T>
T* maximum(T* a, T* b)
{
    std::cout << "+++\n";
    puts(__PRETTY_FUNCTION__);
    return *a < *b ? b : a;
}

const char* maximum(const char* a, const char* b)
{
    puts(__PRETTY_FUNCTION__);
    return std::strcmp(a, b) < 0 ? b : a;
}

TEST_CASE("function templates")
{
    REQUIRE(maximum<int>(4, 6) == 6);

    REQUIRE(maximum(4.5f, static_cast<float>(3.14)) == Approx(4.5));

    REQUIRE(maximum<std::string>("abc"s, "def") == "def"s);

    int x = 10;
    int y = 20;

    auto result = maximum(&x, &y);

    REQUIRE(*result == 20);

    REQUIRE(maximum("abc", "aef") == "aef"s);

    int (*ptr_fun)(int, int) = &maximum<int>;
}

///////////////////////////////////////////////////
// how to define return type from function template

namespace ver_1
{
    template <typename TResult, typename T1, typename T2>
    TResult add(const T1& a, const T2& b)
    {
        return a + b;
    }
}

namespace ver_2
{
    template <typename T1, typename T2>
    auto add(const T1& a, const T2& b)
    {
        return a + b;
    }

    template <typename TContainer>
    decltype(auto) get_nth(TContainer& container, size_t nth)
    {
        return container[nth];
    }
}

namespace ver_3
{
    template <typename T1, typename T2>
    std::common_type_t<T1, T2> maximum(const T1& a, const T2& b)
    {
        return a < b ? b : a;
    }
}

namespace ver_4
{
    template <typename T1, typename T2, typename ResultT = std::common_type_t<T1, T2>>
    ResultT maximum(const T1& a, const T2& b)
    {
        return a < b ? b : a;
    }
}

TEST_CASE("return types")
{
    {
        auto result = ver_1::add<int>('a', 'b');
        static_assert(std::is_same_v<decltype(result), int>);
        REQUIRE(result == 97 + 98);
    }

    {
        auto result = ver_2::add(12, 665L);
        static_assert(std::is_same_v<decltype(result), long>);
    }

    {
        auto result = ver_3::maximum(uint8_t {16}, uint64_t {25});
        static_assert(std::is_same_v<decltype(result), uint64_t>);
    }

    {
        auto result = ver_4::maximum("abc", "def"s);
        static_assert(std::is_same_v<decltype(result), std::string>);
    }

    SECTION("auto vs. decltype")
    {
        std::vector<int> vec = {1, 0, 1, 0};

        auto item1 = vec[0]; // int
        decltype(auto) item2 = vec[0]; // int&

        item2 = 0;
        REQUIRE(vec[0] == 0);

        static_assert(std::is_same_v<decltype(vec[0]), int&>);

        ver_2::get_nth(vec, 1) = 1;
    }
}

/////////////////////////////////////////////////////////////////////////
/// CLASS TEMPLATES


template<typename T>
class Holder
{
    T value_;
public:
    Holder(const T& val) : value_(val)
    {}

    Holder(T&& val) : value_(std::move(val))
    {}

    T& value()
    {
        return value_;
    }

    const T& value() const
    {
        return value_;
    } 
};


/////////////////
// Array

template <typename T, size_t N>
struct Array
{
    T items_[N];

    using iterator = T*;
    using const_iterator = const T*;
    using value_type = T;
    using reference = T&;
    using const_reference = const T&;

    iterator begin()
    {
        return items_;
    }

    iterator end()
    {
        return items_ + N;
    }

    const_iterator begin() const 
    {
        return items_;
    }

    const_iterator end() const
    {
        return items_ + N;
    }

    size_t size() const
    {
        return N;
    }

    reference operator[](size_t index)
    {
        return items_[index];
    }

    const_reference operator[](size_t index) const
    {
        return items_[index];
    }
};

TEST_CASE("class templates")
{
    std::cout << "\n-----------------------------------\n";

    Holder<int> h1{42};
    REQUIRE(h1.value() == 42);

    Holder<std::string> h2 = "text"s;
    REQUIRE(h2.value() == "text"s);

    std::vector<int> vec = {1, 2, 3};
    Holder<std::vector<int>> h3 = std::move(vec);
    REQUIRE(h3.value() == std::vector{1, 2, 3});

    const Holder<int> h4{42};
    REQUIRE(h4.value() == 42);

    Array<int, 4> arr = {1, 2, 3, 4};

    for(const auto& item : arr)
        std::cout << item << " ";
    std::cout << "\n";
}

struct Rectangle
{
    int width, height;
    double rotate;

    int area() const
    {
        return width * height;
    }
};

TEST_CASE("Aggregates")
{
    Rectangle r1{10, 20, 45.0};
    Rectangle r2{50, 5};
    Rectangle r3{};

    Rectangle r4 = {1, 4, 45.0};

    static_assert(std::is_aggregate_v<Rectangle>);

    int tab[10] = {};
}

template <typename T, template<typename, typename> class Container>
class Stack
{
public:
    Container<T, std::allocator<T>> items_;
};

TEST_CASE("template as template params")
{
    Stack<int, std::vector> s1;
    Stack<int, std::list> s2;
}


