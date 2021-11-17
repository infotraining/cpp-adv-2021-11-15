#include "catch.hpp"
#include "gadget.hpp"
#include <memory>
#include <string>

template <typename T>
class UniquePtr
{
    T* ptr_;

public:
    UniquePtr(nullptr_t) noexcept
        : ptr_ {nullptr}
    {
    }

    UniquePtr() noexcept
        : ptr_ {nullptr}
    {
    }

    explicit UniquePtr(T* ptr) noexcept
        : ptr_ {ptr}
    {
    }

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // move constructor
    UniquePtr(UniquePtr&& source) noexcept
        : ptr_ {source.ptr_}
    {
        source.ptr_ = nullptr;
    }

    // move assignment operator
    UniquePtr& operator=(UniquePtr&& source) noexcept
    {
        if (this != &source)
        {
            delete ptr_; // deleting previous resource

            ptr_ = source.ptr_;
            source.ptr_ = nullptr;
        }

        return *this;
    }

    ~UniquePtr() noexcept
    {
        delete ptr_;
    }

    explicit operator bool() const noexcept
    {
        return ptr_ != nullptr;
    }

    T* get() const noexcept
    {
        return ptr_;
    }

    T* operator->() const noexcept
    {
        return ptr_;
    }

    T& operator*() const noexcept
    {
        return ptr_;
    }
};

TEST_CASE("2---")
{
    std::cout << "\n--------------------------\n\n";
}

UniquePtr<Gadget> create_gadget()
{
    static int id_gen = 0;
    const int id = ++id_gen;

    UniquePtr<Gadget> ptr_g{new Gadget{id, "gadget-" + std::to_string(id)}};

    return ptr_g;
}

void use(UniquePtr<Gadget> ptr_g)
{
    if (ptr_g)
        ptr_g->use();
}

// template <typename T>
// UniquePtr<T> MakeUnique()
// {
//     return UniquePtr<T>{new T()};
// }

// template <typename T, typename TArg>
// UniquePtr<T> MakeUnique(TArg&& arg)
// {
//     return UniquePtr<T>{new T(std::forward<TArg>(arg))};
// }

// template <typename T, typename TArg1, typename TArg2>
// UniquePtr<T> MakeUnique(TArg1&& arg1, TArg2&& arg2)
// {
//     return UniquePtr<T>{new T(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2))};
// }


template <typename T, typename... TArgs>
UniquePtr<T> MakeUnique(TArgs&&... arg)
{
    return UniquePtr<T>{new T(std::forward<TArgs>(args)...)};
}

TEST_CASE("move semantics - UniquePtr")
{
    UniquePtr<Gadget> pg1 = MakeUnique<Gadget>(1, "ipad");
    pg1->use();

    UniquePtr<Gadget> pg2 = std::move(pg1); // explicit
    pg2->use();

    UniquePtr<Gadget> pg3 = MakeUnique<Gadget>(2, "smartwatch");
    pg3->use();

    pg3 = std::move(pg3); // explicit move

    UniquePtr<Gadget> pg4 = MakeUnique<Gadget>(3, "smart-tv"); // implicit move
    pg4->use();

    pg4 = MakeUnique<Gadget>(4, "roomba"); // implicit move
    pg4->use();

    UniquePtr<Gadget> pg5 = create_gadget();
    pg5->use();

    use(std::move(pg5)); // explicit move
    use(create_gadget()); // implicit move

    std::cout << "****\n";
}

TEST_CASE("container of moveable object")
{
    std::vector<UniquePtr<Gadget>> gadgets;

    gadgets.push_back(create_gadget());
    gadgets.push_back(create_gadget());

    UniquePtr<Gadget> g = create_gadget();
    gadgets.push_back(std::move(g));

    for(const auto& g : gadgets)
        g->use();
}

TEST_CASE("perfect forwarding & memory leaks")
{
    std::vector<Gadget> gadgets;
    gadgets.push_back(Gadget{1, "ipad"});
    gadgets.emplace_back(2, "smart-tv");

    SECTION("avoid memory leaks with make_unique")
    {
        std::vector<std::unique_ptr<Gadget>> gadget_ptrs;

        gadget_ptrs.push_back(std::make_unique<Gadget>(1, "ipad"));
        //gadget_ptrs.emplace_back(new Gadget{2, "smart-tv"}); // potential memory leak
        gadget_ptrs.emplace_back(std::make_unique<Gadget>(3, "smart-watch")); // leak-free 
    }
}