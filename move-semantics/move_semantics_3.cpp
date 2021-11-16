#include "catch.hpp"
#include <iostream>

////////////////////////////////////////////////////////////////////////////
// Data - class with copy & move semantics (user provided implementation)

class Data
{
    std::string name_;
    int* data_;
    size_t size_;

public:
    using iterator = int*;
    using const_iterator = const int*;

    Data(std::string name, std::initializer_list<int> list)
        : name_ {std::move(name)}
        , data_ {new int[list.size()]}
        , size_ {list.size()}
    {
        std::copy(list.begin(), list.end(), data_);

        std::cout << "Data(" << name_ << ")\n";
    }

    Data(const Data& other)
        : name_(other.name_)
        , data_ {new int[other.size_]}
        , size_(other.size_)
    {
        std::copy(other.begin(), other.end(), data_);

        std::cout << "Data(" << name_ << ": cc)\n";
    }

    Data& operator=(const Data& other)
    {
        Data temp(other);
        swap(temp);

        std::cout << "Data=(" << name_ << ": cc)\n";

        return *this;
    }

    /////////////////////////////////////////////////
    // move constructor
    Data(Data&& other) noexcept
        : name_ {std::move(other.name_)}
        , data_ {other.data_}
        , size_ {other.size_}
    {
        other.data_ = nullptr;
        other.size_ = 0; // extra

        std::cout << "Data(" << name_ << ": mv)\n";
    }

    /////////////////////////////////////////////////
    // move assignment
    Data& operator=(Data&& other) noexcept(noexcept(other.swap(std::declval<Data>())))
    {
        if (this != &other)
        {
            // delete[] data_; // release previous state

            // name_ = std::move(other.name_);

            // data_ = other.data_;
            // other.data_ = nullptr;

            // size_ = other.size_;
            // other.size_ = 0; // extra

            // simplified version with move&swap idiom
            Data temp = std::move(other);
            swap(temp);

            std::cout << "Data=(" << name_ << ": mv)\n";
        }

        return *this;
    }

    ~Data() noexcept
    {
        delete[] data_;
    }

    void swap(Data& other) noexcept(noexcept(std::declval<std::string>().swap(std::declval<std::string>())))
    {
        name_.swap(other.name_);
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
    }

    iterator begin()
    {
        return data_;
    }

    iterator end()
    {
        return data_ + size_;
    }

    const_iterator begin() const
    {
        return data_;
    }

    const_iterator end() const
    {
        return data_ + size_;
    }
};

Data create_data_set()
{
    Data ds {"data-set-one", {54, 6, 34, 235, 64356, 235, 23}};

    return ds;
}

namespace LegacyCode
{
    Data* create_data_set()
    {
        Data* ptr = new Data("data-set-one", {54, 6, 34, 235, 64356, 235, 23});

        return ptr;
    }
}

template <typename TContainer>
void print(const std::string& prefix, const TContainer& container)
{
    std::cout << prefix << " - [ ";
    for (const auto& item : container)
    {
        std::cout << item << " ";
    }
    std::cout << "]\n";
}

TEST_CASE("3---")
{
    std::cout << "\n--------------------------\n\n";
}

TEST_CASE("Data & move semantics")
{
    Data data1 {"data1", {1, 2, 3, 4, 5}};

    Data backup = data1; // copy using copy constructor
    print("backup", backup);

    Data target = std::move(data1); // move using move constructor
    print("target", target);

    target = Data {"data2", {34, 235, 643, 23, 46, 665}};
    print("target", target);
}

class DataSet
{
    std::string name_;
    Data row1_;
    Data row2_;

public:
    DataSet(std::string name, Data row1, Data row2)
        : name_(std::move(name))
        , row1_(std::move(row1))
        , row2_(std::move(row2))
    {
    }

    void print_rows() const
    {
        std::cout << name_ << "\n";
        print("1", row1_);
        print("2", row2_);
    }
};

TEST_CASE("DataSet")
{
    Data d {"a", {1, 2, 3}};
    DataSet ds("dataset", d, Data {"b", {3, 4, 5}});
    print("d", d);
    ds.print_rows();

    std::cout << "\n&&&&&&&&&&&&&&&&&&\n";

    DataSet target = std::move(ds);
    target.print_rows();

    target = DataSet {"new-dataset", d, Data {"c", {6, 7, 8, 9, 0, 3}}};
    target.print_rows();

    DataSet backup = target;
    backup.print_rows();
}

struct AllByDefault
{
    int id;
    std::string str;
    std::vector<int> vec;
    DataSet ds;
};

struct RuleOfFive
{
    int id;
    std::string str;
    std::vector<int> vec;
    DataSet ds;

    ~RuleOfFive() { std::cout << "Log: object being destroyed!\n"; }
    RuleOfFive(const RuleOfFive&) = default;
    RuleOfFive& operator=(const RuleOfFive&) = default;
    RuleOfFive(RuleOfFive&&) = default;
    RuleOfFive& operator=(RuleOfFive&&) = default;
};

TEST_CASE("AllByDefault")
{
    AllByDefault abd {665, "abd", {1, 2, 3}, DataSet {"ds", Data {"a", {1, 2, 3}}, Data {"b", {4, 5, 6}}}};

    AllByDefault backup = abd;

    std::cout << "\n##############\n";

    AllByDefault target = std::move(abd);
}

namespace ModernCpp
{
    class Data
    {
        std::string name_;
        std::vector<int> data_;

    public:
        using iterator = std::vector<int>::iterator;
        using const_iterator = std::vector<int>::const_iterator;

        Data(std::string name, std::initializer_list<int> list)
            : name_ {std::move(name)}
            , data_ {list}
        {
            std::cout << "Data(" << name_ << ")\n";
        }

        void swap(Data& other)
        {
            name_.swap(other.name_);
            data_.swap(other.data_);
        }

        iterator begin()
        {
            return data_.begin();
        }

        iterator end()
        {
            return data_.end();
        }

        const_iterator begin() const
        {
            return data_.begin();
        }

        const_iterator end() const
        {
            return data_.end();
        }
    };
}

/////////////////////////////////////////////////////////////

struct Point
{
    int x, y;
};

class Complex
{
    double re_ {};
    double img_ {};

public:
    Complex() = default;

    Complex(double r, double i)
        : re_ {r}
        , img_ {i}
    {
    }
};

TEST_CASE("uniform init syntax")
{
    SECTION("C++98")
    {
        int x1 = 10;
        int x2(10);
        int x3 = int(20);
        // int x4(); // most vexing parse

        Point pt1 {1, 2};
        Point pt2 = {1, 6};

        Complex c(1.1, 5.5);

        int tab1[5] = {1, 2, 3, 4, 5};
        int tab2[5] {1, 2, 3, 4, 6};

        std::vector<int> vec;
        vec.push_back(1);
        vec.push_back(2);
    }

    SECTION("C++11")
    {
        int x1 = 10;
        int x2 {10};
        int x3 = int {20};
        int x4 {}; // int == 0
        int* ptr {};
        REQUIRE(ptr == nullptr);

        Point pt1 {1, 2};
        Point pt2 = {1, 6};

        Complex c {1.1, 5.5};

        int tab1[5] = {1, 2, 3, 4, 5};
        int tab2[5] {1, 2, 3, 4, 6};

        const std::vector<int> vec = {1, 2, 3, 4};
    }
}

TEST_CASE("noexcept")
{
    std::cout << "\n\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n\n";

    std::vector<Data> vec;

    vec.push_back(Data {"a", {1, 2, 3}});
    std::cout << "-----\n";
    vec.push_back(Data {"b", {1, 2, 3}});
    std::cout << "-----\n";
    vec.push_back(Data {"c", {1, 2, 3}});
    std::cout << "-----\n";
    vec.push_back(Data {"d", {1, 2, 3}});
    std::cout << "-----\n";
    vec.push_back(Data {"e", {1, 2, 3}});
    std::cout << "-----\n";
    vec.push_back(Data {"f", {1, 2, 3}});
}

std::string foo()
{
    return "abc";
}

class X
{
public:
    X() = delete;

    std::string bar()
    {
        return "abc";
    }
};

TEST_CASE("declval")
{
    decltype(foo()) s; 

    std::vector<decltype(foo())> words;   

    decltype(std::declval<X>().bar()) obj;
}