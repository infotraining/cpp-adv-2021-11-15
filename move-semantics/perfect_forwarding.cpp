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

void use(Gadget& g)
{
    have_fun(g);
}

void use(const Gadget& g)
{
    have_fun(g);
}

void use(Gadget&& g)
{
    have_fun(std::move(g));
}

TEST_CASE("using gadget")
{
    Gadget g {1, "g"};
    const Gadget cg {2, "const g"};

    use(g);
    use(cg);
    use(Gadget {3, "temporary gadget"});
}
