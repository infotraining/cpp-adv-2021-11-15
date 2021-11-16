#include "catch.hpp"
#include "gadget.hpp"
#include <deque>


#ifdef _MSC_VER
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

TEST_CASE("4---")
{
    std::cout << "\n--------------------------\n\n";
}

////////////////////////////////////////////////////////
///  PERFECT FORWARDING

void have_fun(Gadget& g)
{
    puts(__PRETTY_FUNCTION__);
    g.use();
}

void have_fun(const Gadget& cg)
{
    puts(__PRETTY_FUNCTION__);
    cg.use();
}

void have_fun(Gadget&& g)
{
    puts(__PRETTY_FUNCTION__);
    g.use();
}

// void use(Gadget& g1)
// {
//     have_fun(g1);
// }

// void use(const Gadget& cg)
// {
//     have_fun(cg);
// }

// void use(Gadget&& g)
// {
//     have_fun(std::move(g));
// }

//////////////////////////////
// use with perfect forward

template <typename G>
void use(G&& g)
{
    have_fun(std::forward<G>(g));
}

namespace Explain
{
    template <typename G>
    void use(G&& g)
    {
        if constexpr(std::is_reference_v<G>)       
            have_fun(g);
        else
            have_fun(std::move(g));
    }
}

template <typename G1, typename G2>
void use(G1&& g1, G2&& g2)
{
    have_fun(std::forward<G1>(g1));
    have_fun(std::forward<G2>(g2));
}

TEST_CASE("using gadget")
{
    Gadget g {1, "g"};
    const Gadget cg {2, "const g"};

    use(g, g);
    use(g, cg);
    use(cg, g);
    use(cg, cg);
    use(Gadget {3, "temporary gadget"}, g);
    use(g, Gadget {3, "temporary gadget"});
    use(Gadget {3, "temporary gadget"}, cg);
    use(cg, Gadget {3, "temporary gadget"});
    use(Gadget {3, "temporary gadget"}, Gadget {3, "temporary gadget"});
}

namespace Explain
{
    void foo(Gadget&& g)
    {
    }

    template <typename T>
    void bar(T& b)
    {
    }
}

TEST_CASE("explain perfect forwarding")
{
    Gadget g;
    // Explain::foo(g);

    Explain::bar(g);
    Explain::bar(Gadget {});
}

template <typename T>
void deduce1(T arg)
{
    puts(__PRETTY_FUNCTION__);
}

template <typename T>
void deduce2(T& arg)
{
    puts(__PRETTY_FUNCTION__);
}

template <typename T>
void deduce3(T&& arg)
{
    puts(__PRETTY_FUNCTION__);
}

TEST_CASE("type deduction - auto")
{
    std::cout << "\n-----------------------------\n";

    int x = 10;
    const int cx = 10;
    int& ref_x = x;
    const int& cref_x = cx;
    int tab[10] = {};

    SECTION("auto - 1st case - without &")
    {
        auto ax1 = 10;
        auto ax2 = x; // int
        deduce1(x);

        auto ax3 = cx; // int
        deduce1(cx);

        auto ax4 = ref_x; // int
        deduce1(ref_x);

        auto ax5 = cref_x; // int
        deduce1(cref_x);

        auto ax6 = tab; // int*
        deduce1(tab);

        auto ax7 = &x; // int*

        auto ax8 = &cx; // const int*
    }

    SECTION("auto - 2nd case - with &")
    {
        auto& ax1 = x; // int&
        deduce2(x);
        auto& ax2 = cx; // const int&
        deduce2(cx);
        auto& ax3 = ref_x; // int&
        auto& ax4 = cref_x; // const int&
        auto& ax5 = tab; // int(&)[10]
    }

    SECTION("auto - 3rd case - with &&")
    {
        auto&& ax1 = x; // lvalue -> auto&& -> auto& -> int&
        deduce3(x);

        auto&& ax2 = 10; // rvalue -> auto&& -> int&&
        deduce3(10);
    }
}

TEST_CASE("reference collapsing")
{
    int x = 10;
    deduce2<int&&>(x);
}

template <typename T>
class Queue
{
    std::deque<T> q_;
public:
    // void push(const T& item)
    // {
    //     q_.push_front(item); // item is copyied into queue
    // }

    // void push(T&& item)
    // {
    //     q_.push_front(std::move(item)); // moved into queue
    // }

    template <typename TItem>
    void push(TItem&& item)
    {
        q_.push_front(std::forward<TItem>(item)); // item is forwarded into queue
    }

    template <typename TArg1, typename TArg2>
    void emplace(TArg1&& arg1, TArg2&& arg2)
    {
        q_.emplace_back(std::forward<TArg1>(arg1), std::forward<TArg2>(arg2));
    }

    void pop(T& item)
    {
        if constexpr(std::is_nothrow_move_assignable_v<T>)
        {
            item = std::move(q_.back()); 
        }
        else
        {
            item = q_.back(); 
        }
        
        q_.pop_back();
    }
};

TEST_CASE("Queue")
{
    Queue<Gadget> q;

    Gadget g{1, "ipad"};
    q.push(g);
    q.push(Gadget{2, "smart-tv"});
    q.emplace(3, "smart-watch");

    Gadget g;
    q.pop(g);
}
