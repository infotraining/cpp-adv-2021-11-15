#include "utils.hpp"
#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

#include "catch.hpp"

// https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-resource

using namespace std;
using namespace Utils;

namespace LegacyCode
{
    // forward declarations
    Gadget* get_gadget(const std::string& name);
    void use(Gadget* g);
    void use_gadget(Gadget* g);

    // definitions
    Gadget* get_gadget(const std::string& name)
    {
        static int id = 665;
        return new Gadget(++id, name);
    }

    void use(Gadget* g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";

        delete g;
    }

    void use_gadget(Gadget* g)
    {
        // if (g)
        //     std::cout << "Using " << g->name() << "\n"; // SEGFAULT
    }
}

namespace ModernCpp
{
    std::unique_ptr<Gadget> get_gadget(const std::string& name);
    void use(std::unique_ptr<Gadget> g);
    void use_gadget(Gadget* g);

    std::unique_ptr<Gadget> get_gadget(const std::string& name)
    {
        static int id = 665;
        return std::make_unique<Gadget>(++id, name);
    }

    void use(std::unique_ptr<Gadget> g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }

    void use_gadget(Gadget* g)
    {
        if (g)
            std::cout << "Using " << g->name() << "\n";
    }
}

TEST_CASE("Modern C++ with dynamic memory")
{
    using namespace ModernCpp;

    {
        std::unique_ptr<Gadget> g = get_gadget("ipad");

        use_gadget(g.get());
    }

    {
        use(get_gadget("ipad"));
    }

    {
        std::unique_ptr<Gadget> g = get_gadget("smart-tv");
        use(std::move(g));
    }

    {
        std::unique_ptr<Gadget> g = get_gadget("smart-tv");

        LegacyCode::use(g.release());
        REQUIRE(g == nullptr);
    }

    {
        std::unique_ptr<Gadget> g = get_gadget("smart-tv");

        g.reset();

        // g is already destroyed
    }
}

TEST_CASE("Legacy hell with dynamic memory")
{
    using namespace LegacyCode;

    {
        Gadget* g = get_gadget("ipad");

        use_gadget(g);
    } // memory leak

    {
        use_gadget(get_gadget("ipad"));
    } // memory leak

    {
        Gadget* g = get_gadget("ipad");

        use(g);

        //std::cout << g->name() << std::endl; // UB - use after delete
    }
}

TEST_CASE("unique_ptr with std containers")
{
    std::vector<std::unique_ptr<Gadget>> gadgets;

    gadgets.push_back(std::make_unique<Gadget>(1, "ipad"));
    gadgets.push_back(ModernCpp::get_gadget("smart-tv"));

    std::unique_ptr<Gadget> g = ModernCpp::get_gadget("smart-watch");
    gadgets.push_back(std::move(g));

    for (const auto& g : gadgets)
        std::cout << g->name() << " ";
    std::cout << "\n";
}

class Owner
{
    std::string name_;
    std::unique_ptr<Gadget> gadget_;

public:
    Owner(std::string name, std::unique_ptr<Gadget> g)
        : name_(std::move(name))
        , gadget_(std::move(g))
    {
    }

    void use() const
    {
        if (gadget_)
            std::cout << "Using " << gadget_->name() << "\n";
    }
};

TEST_CASE("Owner")
{
    Owner jk {"Jan Kowalski", ModernCpp::get_gadget("ipad3")};
    jk.use();

    Owner other = std::move(jk);
    other.use();
}

void may_throw()
{
    //throw 42;
}

class Stream
{
public:
    void open()
    {
        std::cout << "stream is opened...\n";
    }

    void close()
    {
        std::cout << "stream is closed...\n";
    }
};

TEST_CASE("custom deallocator")
{
    SECTION("Legacy C++")
    {
        FILE* f = fopen("abc.txt", "w+");

        fputs("text", f);
        may_throw();

        fclose(f);
    } // file not closed - leak

    SECTION("Modern C++")
    {
        using FileCloser = int(*)(FILE*);
        std::unique_ptr<FILE, FileCloser> f{fopen("abc.txt", "w+"), &fclose};

        fputs("text", f.get());
        may_throw();
    } // file is automatically closed

    SECTION("Stream with unique_ptr")
    {
        Stream stream;
        stream.open();

        auto stream_closer = [](Stream* s) { if (s) s->close(); };
        std::unique_ptr<Stream, decltype(stream_closer)> guard{&stream, stream_closer};

        //...
    }
}

namespace LegacyCode
{
    int* create_data()
    {
        return new int[1024] {1, 2, 3, 4};
    };
}

TEST_CASE("unique_ptr<T[]>")
{
    std::unique_ptr<int[]> tab{LegacyCode::create_data()};

    tab[100] = 562;
    tab[101] = 42;
} // tab is destroyed and calls delete[]

TEST_CASE("shared_ptr & weak_ptrs")
{
    std::cout << "\n******************\n";
    
    auto sp1 = std::make_shared<Gadget>(1, "ipad1");
    REQUIRE(sp1.use_count() == 1);

    auto sp2 = sp1;
    REQUIRE(sp1.use_count() == 2);

    std::weak_ptr<Gadget> wp = sp1;
    REQUIRE(sp1.use_count() == 2);

    sp1.reset();
    sp2.reset();

    REQUIRE(wp.use_count() == 0);
    
    if (shared_ptr<Gadget> sp = wp.lock())
    {
        std::cout << sp->name() << "\n";
    }

    std::cout << "\n******************\n";
}

class Device : public std::enable_shared_from_this<Device>
{
public:
    std::shared_ptr<Device> get_dev_ptr()
    {
        return shared_from_this();
    }
};

TEST_CASE("enable_shared_from_this")
{
    std::vector<std::shared_ptr<Device>> devs;

    auto dev = std::make_shared<Device>();

    devs.push_back(dev->get_dev_ptr());
}

TEST_CASE("unique_ptr & shared_ptr")
{
    std::shared_ptr<Gadget> sp = ModernCpp::get_gadget("ipad4");

    auto sp2 = sp;
}