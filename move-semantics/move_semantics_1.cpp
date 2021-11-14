#include "catch.hpp"
#include <deque>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace Catch::Matchers;

std::string full_name(const std::string& first_name, const std::string& last_name)
{
    return first_name + " " + last_name;
}

TEST_CASE("reference binding")
{
    std::string name = "jan";

    SECTION("C++98")
    {
        // TODO
    }

    SECTION("C++11")
    {
        // TODO
    }
}

