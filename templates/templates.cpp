#include "catch.hpp"
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <optional>
#include <set>
#include <string>
#include <tuple>
#include <vector>

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

template <typename T>
class Holder
{
    T value_;

public:
    Holder(const T& val)
        : value_(val)
    {
    }

    Holder(T&& val)
        : value_(std::move(val))
    {
    }

    template <typename... Args>
    Holder(std::in_place_t, Args&&... args)
        : value_(std::forward<Args>(args)...)
    {
    }

    T& value() noexcept
    {
        return value_;
    }

    const T& value() const noexcept
    {
        return value_;
    }
};

template <typename T>
class Holder<T*>
{
    std::unique_ptr<T> value_;

public:
    Holder(T* val)
        : value_(val)
    {
        assert(value_ != nullptr);
    }

    T& value() noexcept
    {
        return *value_;
    }

    const T& value() const noexcept
    {
        return *value_;
    }

    T* get() const
    {
        return value_.get();
    }
};

template <>
class Holder<const char*>
{
    std::string value_;

public:
    Holder(const char* val)
        : value_(val)
    {
    }

    std::string& value() noexcept
    {
        return value_;
    }

    const std::string& value() const noexcept
    {
        return value_;
    }
};

TEST_CASE("class templates")
{
    std::cout << "\n-----------------------------------\n";

    Holder<int> h1 {42};
    REQUIRE(h1.value() == 42);

    Holder<std::string> h2 = "text"s;
    REQUIRE(h2.value() == "text"s);

    std::vector<int> vec = {1, 2, 3};
    Holder<std::vector<int>> h3 = std::move(vec);
    REQUIRE(h3.value() == std::vector {1, 2, 3});

    const Holder<int> h4 {42};
    REQUIRE(h4.value() == 42);

    Holder<int*> h5(new int(13));
    REQUIRE(h5.value() == 13);
    REQUIRE(h5.get() != nullptr);

    Holder<const char*> h6("text");
    REQUIRE(h6.value() == "text"s);

    std::optional<std::string> os1("aaa");
    std::optional<std::string> os2(std::in_place, 'a', 3);

    Holder<std::string> h7(std::in_place, 3, 'a'); // Holder(in_place, 3, 'a') -> std::string(3, 'a')
    REQUIRE(h7.value() == "aaa"s);

    Holder<std::string> h8(std::in_place, "aabb", 2);
    REQUIRE(h8.value() == "aa");
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
    Rectangle r1 {10, 20, 45.0};
    Rectangle r2 {50, 5};
    Rectangle r3 {};

    Rectangle r4 = {1, 4, 45.0};

    static_assert(std::is_aggregate_v<Rectangle>);

    int tab[10] = {};
}

template <typename T, template <typename, typename> class Container>
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

    constexpr const_iterator begin() const
    {
        return items_;
    }

    constexpr const_iterator end() const
    {
        return items_ + N;
    }

    constexpr size_t size() const
    {
        return N;
    }

    reference operator[](size_t index)
    {
        return items_[index];
    }

    constexpr const_reference operator[](size_t index) const
    {
        return items_[index];
    }
};

TEST_CASE("Array")
{
    Array<int, 4> arr = {1, 2, 3, 4};

    for (const auto& item : arr)
        std::cout << item << " ";
    std::cout << "\n";
}

/////////////////////////////////
// aliases

template <typename TValue>
using Dictionary = std::map<std::string, TValue>;

template <typename T>
struct Identity
{
    using type = T;
};

template <typename T>
using Identity_t = typename Identity<T>::type;

TEST_CASE("template aliases")
{
    Dictionary<int> dict;

    dict.emplace("five", 5);
    dict.emplace("six", 6);

    static_assert(std::is_same_v<Identity_t<int>, int>);
}

//////////////////////////////////////
// template variables

template <typename T>
const T pi(3.14153453645645654456);

template <class T>
inline constexpr bool IsIntegral_v = is_integral<T>::value;

TEST_CASE("template variable")
{
    std::cout << pi<float> << std::endl;
}

////////////////////////////////////////////
// variadic templates

template <typename... Ts>
struct Row
{
    std::tuple<Ts...> data;
};

namespace BeforeCpp17
{
    void print()
    {
        std::cout << "\n";
    }

    template <typename THead, typename... TTail>
    void print(const THead& head, const TTail&... tail)
    {
        std::cout << head << " ";
        print(tail...);
    }
}

template <typename THead, typename... TTail>
void print(const THead& head, const TTail&... tail)
{
    std::cout << head << " ";

    if constexpr (sizeof...(tail) > 0)
    {
        print(tail...);
    }
    else
        std::cout << "\n";
}

TEST_CASE("variadic templates")
{
    Row<int, double, std::string> r1 {{1, 3.14, "test"}};

    print(1, 3.14, "text"s);
}

void foo(int i, double d, const std::string& s)
{
    std::cout << "foo(" << i << ", " << d << ", " << s << ")\n";
}

template <typename F, typename... TArgs>
decltype(auto) call(F f, TArgs&&... args)
{
    return f(std::forward<TArgs>(args)...);
}

TEST_CASE("call")
{
    call(foo, 1, 3.14, "pi");
}

//////////////////////////////////////////////////////
// std::tuples

template <typename TContainer>
std::tuple<int, int, double> calc_stats(const TContainer& container)
{
    // decltype(begin(container)) min_pos, max_pos;
    // std::tie(min_pos, max_pos) = std::minmax_element(begin(container), end(container));

    auto [min_pos, max_pos] = std::minmax_element(begin(container), end(container)); // since C++17
    double avg = std::accumulate(begin(container), end(container), 0.0) / std::size(container);

    return {*min_pos, *max_pos, avg};
}

TEST_CASE("tuples")
{
    std::vector<int> vec = {43, 665, 23, 645, 7568, 23, 654, 2, 1};

    {
        int min, max;

        std::tie(min, max, std::ignore) = calc_stats(vec);
    }

    auto [min, max, _] = calc_stats(vec); // since C++17

    REQUIRE(min == 1);
    REQUIRE(max == 7568);
}

struct Person
{
    std::string fname;
    std::string lname;
    int age;

    auto tied() const
    {
        return std::tie(fname, lname, age);
    }

    bool operator==(const Person& p) const
    {
        return tied() == p.tied();
    }

    bool operator<(const Person& p) const
    {
        return std::tie(fname, lname, age) < std::tie(p.fname, p.lname, p.age);
    }
};

TEST_CASE("Person ==")
{
    Person p1 {"Jan", "Kowalski", 44};
    Person p2 {"Jan", "Kowalski", 44};

    REQUIRE(p1 == p2);
}

// implementation for non-integral types
template <typename T, typename = std::enable_if_t<!std::is_integral_v<T>>>
T calculate(T x)
{
    std::cout << "calculate(T: " << x << ")\n";

    return x * x;
}

// implementation for integral types
template <typename T, typename = void, typename = std::enable_if_t<std::is_integral_v<T>>>
int calculate(T x)
{
    std::cout << "calculate(integral: " << x << ")\n";

    return x * x;
}

// std::integral auto calculate(std::integral auto i)
// {
//     std::cout << "calculate(integral: " << x << ")\n";

//     return x * x;
// }

////////
// type traits

namespace Explain
{
    template <typename T>
    struct IsIntegral
    {
        static constexpr bool value = false;
    };

    template <>
    struct IsIntegral<int>
    {
        static constexpr bool value = true;
    };

    template <>
    struct IsIntegral<short>
    {
        static constexpr bool value = true;
    };

    template <>
    struct IsIntegral<long>
    {
        static constexpr bool value = true;
    };

    template <typename T>
    constexpr static bool IsIntegral_v = IsIntegral<T>::value;
}

TEST_CASE("enable_if + traits")
{
    int x = 42;
    calculate(x);

    short s = 42;
    calculate(s);

    calculate(3.14);

    static_assert(Explain::IsIntegral<double>::value == false);
    static_assert(Explain::IsIntegral_v<std::string> == false);
    static_assert(std::is_integral<int>::value);
}