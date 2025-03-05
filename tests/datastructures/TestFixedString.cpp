// SPDX-License-Identifier: GPL-3.0-only

#include <catch_amalgamated.hpp>
#include "datastructures/FixedString.h"
#include "TestCommons.h"

using namespace tpunkt;

TEST_CASE("FixedString Tests")
{
    TEST_INIT();

    SECTION("DefaultConstruction")
    {
        FixedString<10> str;
        REQUIRE(str.size() == 0);
        REQUIRE(std::strcmp(str.c_str(), "") == 0);
    }

    SECTION("ConstructionWithCString")
    {
        FixedString<10> str("Hello");
        REQUIRE(str.size() == 5);
        REQUIRE(str == "Hello");
    }

    SECTION("Truncation")
    {
        FixedString<10> str("This is a very long string");
        REQUIRE(str.size() == 10);
        REQUIRE(std::strncmp(str.c_str(), "This is a ", 10) == 0);
        REQUIRE(str.c_str()[ 10 ] == '\0'); // Ensure null termination
    }

    SECTION("AssignCString")
    {
        FixedString<10> str;
        str = "Test";
        REQUIRE(str.size() == 4);
        REQUIRE(std::strcmp(str.c_str(), "Test") == 0);

        str = "A much longer string";
        REQUIRE(str.size() == 10);
        REQUIRE(std::strncmp(str.c_str(), "A much lon", 10) == 0);
    }

    SECTION("AssignFixedString")
    {
        FixedString<10> str1("Hello");
        FixedString<5> str2;
        str2 = str1;
        REQUIRE(str2.size() == 5);
        REQUIRE(std::strcmp(str2.c_str(), "Hello") == 0);
    }

    SECTION("EqualityComparison")
    {
        FixedString<10> str1("Hello");
        FixedString<10> str2("Hello");
        FixedString<10> str3("World");
        FixedString<5> str4("Long String");
        FixedString<51> str5("Very Long String");

        REQUIRE(str1 == str2);
        REQUIRE(str1 != str4);
        REQUIRE(str4 != str5);
        REQUIRE_FALSE(str1 == str3);
        REQUIRE(str1 == "Hello");
        REQUIRE(str5 == "Very Long String");
    }

    SECTION("InequalityComparison")
    {
        FixedString<10> str1("Hello");
        FixedString<10> str2("World");

        REQUIRE_FALSE(str1 == str2);
        REQUIRE(str1 != str2);
    }

    SECTION("NullAssignment")
    {
        FixedString<10> str;
        str = nullptr; // Simulate passing a null pointer
        REQUIRE(str == "");
    }

    SECTION("SizeCalculation")
    {
        FixedString<10> str("Test");
        REQUIRE(str.size() == 4);

        FixedString<10> emptyStr;
        REQUIRE(emptyStr.size() == 0);
    }

    SECTION("DataAccess")
    {
        FixedString<10> str("Hello");
        char* data = str.data();
        REQUIRE(data[ 0 ] == 'H');
        data[ 0 ] = 'J'; // Modify via `data()`
        REQUIRE(std::strcmp(str.c_str(), "Jello") == 0);
    }

    SECTION("Copy constructor")
    {
        FixedString<10> str("Hello");
        FixedString str2{str};

        REQUIRE(str2 == str);
        REQUIRE(str2 == "Hello");

        FixedString<15> str3{str}; // Different template
        REQUIRE(str3 == str);
        REQUIRE(str3 == "Hello");
    }
}