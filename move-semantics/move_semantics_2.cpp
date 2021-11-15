#include "catch.hpp"
#include "gadget.hpp"
#include <memory>
#include <string>

template <typename T>
class UniquePtr
{
    T* ptr_;

public:
    UniquePtr(nullptr_t)
        : ptr_ {nullptr}
    {
    }

    UniquePtr()
        : ptr_ {nullptr}
    {
    }

    explicit UniquePtr(T* ptr)
        : ptr_ {ptr}
    {
    }

    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // move constructor
    UniquePtr(UniquePtr&& source)
        : ptr_ {source.ptr_}
    {
        source.ptr_ = nullptr;
    }

    // move assignment operator
    UniquePtr& operator=(UniquePtr&& source)
    {
        if (this != &source)
        {
            delete ptr_; // deleting previous resource

            ptr_ = source.ptr_;
            source.ptr_ = nullptr;
        }

        return *this;
    }

    ~UniquePtr()
    {
        delete ptr_;
    }

    explicit operator bool() const
    {
        return ptr_ != nullptr;
    }

    T* get() const
    {
        return ptr_;
    }

    T* operator->() const
    {
        return ptr_;
    }

    T& operator*() const
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

TEST_CASE("move semantics - UniquePtr")
{
    UniquePtr<Gadget> pg1 {new Gadget {1, "ipad"}};
    pg1->use();

    UniquePtr<Gadget> pg2 = std::move(pg1); // explicit
    pg2->use();

    UniquePtr<Gadget> pg3 {new Gadget {2, "smartwatch"}};
    pg3->use();

    pg3 = std::move(pg3); // explicit move

    UniquePtr<Gadget> pg4 = UniquePtr<Gadget>(new Gadget{3, "smart-tv"}); // implicit move
    pg4->use();

    pg4 = UniquePtr<Gadget>{new Gadget{4, "roomba"}}; // implicit move
    pg4->use();

    UniquePtr<Gadget> pg5 = create_gadget();
    pg5->use();

    use(std::move(pg5)); // explicit move
    use(create_gadget()); // implicit move

    std::cout << "****\n";
}