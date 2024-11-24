#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <iostream>
#include "functions.h"

TEST_CASE("Testing trimText function") {
    auto trim = trimText("start")("end");

    SUBCASE("Valid trim") {
        auto result = trim("This is a start trimmed end text");
        CHECK(result.valueType == " trimmed ");
    }

    SUBCASE("Start marker not found") {
        auto result = trim("This is a trimmed end text");
        CHECK(result.valueType == std::nullopt);
    }

    SUBCASE("End marker not found") {
        auto result = trim("This is a start trimmed text");
        CHECK(result.valueType == std::nullopt);
    }

    SUBCASE("End marker before start marker") {
        auto result = trim("This is a end trimmed start text");
        CHECK(result.valueType == std::nullopt);
    }
}


TEST_CASE("filterText with Maybe<std::string>") {
    SUBCASE("Basic alphabetic input") {
        Maybe<std::string> input{"HelloWorld"};
        auto result = filterText(input.valueType.value());
        REQUIRE(result.valueType.has_value());
        CHECK(result.valueType.value() == "HelloWorld");
    }

    SUBCASE("Input with non-alphabetic characters") {
        Maybe<std::string> input{"Hello, World!"};
        auto result = filterText(input.valueType.value());
        REQUIRE(result.valueType.has_value());
        CHECK(result.valueType.value() == "Hello  World ");
    }

    SUBCASE("Input with valid apostrophes and hyphens") {
        Maybe<std::string> input{"It's a well-known fact."};
        auto result = filterText(input.valueType.value());
        REQUIRE(result.valueType.has_value());
        CHECK(result.valueType.value() == "It's a well-known fact ");
    }

    SUBCASE("Input with invalid apostrophes and hyphens") {
        Maybe<std::string> input{"'start '- end-' middle'"};
        auto result = filterText(input.valueType.value());
        REQUIRE(result.valueType.has_value());
        CHECK(result.valueType.value() == " start    end   middle ");
    }

    SUBCASE("Empty string input") {
        Maybe<std::string> input{""};
        auto result = filterText(input.valueType.value());
        REQUIRE(result.valueType.has_value());
        CHECK(result.valueType.value() == "");
    }

    SUBCASE("All non-alphabetic characters") {
        Maybe<std::string> input{"1234567890!@#$%^&*()_+"};
        auto result = filterText(input.valueType.value());
        REQUIRE(result.valueType.has_value());
        CHECK(result.valueType.value() == "                      ");
    }

    SUBCASE("Input with consecutive special characters") {
        Maybe<std::string> input{"hello--world!!"};
        auto result = filterText(input.valueType.value());
        REQUIRE(result.valueType.has_value());
        CHECK(result.valueType.value() == "hello  world  ");
    }

    SUBCASE("Input without valid characters") {
        Maybe<std::string> input{"!@#$%^&*()"};
        auto result = filterText(input.valueType.value());
        REQUIRE(result.valueType.has_value());
        CHECK(result.valueType.value() == "          ");
    }
}

TEST_CASE("Testing isAlpha function") {
    CHECK(isAlpha('a'));
    CHECK(isAlpha('z'));
    CHECK(isAlpha('A'));
    CHECK(isAlpha('Z'));
    CHECK_FALSE(isAlpha('0'));
    CHECK_FALSE(isAlpha('9'));
    CHECK_FALSE(isAlpha('!'));
    CHECK_FALSE(isAlpha('@'));
    CHECK_FALSE(isAlpha(' '));
    CHECK_FALSE(isAlpha('\n'));
    CHECK_FALSE(isAlpha('\t'));
}

TEST_CASE("String to Upper") {
    SUBCASE("Valid input") {
        Maybe<std::string> input{"hello"};
        auto result = str_toupper(input.valueType.value());
        REQUIRE(result.valueType.has_value());
        CHECK(result.valueType.value() == "HELLO");
    }

    SUBCASE("Empty string") {
        Maybe<std::string> input{""};
        auto result = str_toupper(input.valueType.value());
        REQUIRE(result.valueType.has_value());
        CHECK(result.valueType.value() == "");
    }

    SUBCASE("Mixed case input") {
        Maybe<std::string> input{"HeLlO"};
        auto result = str_toupper(input.valueType.value());
        REQUIRE(result.valueType.has_value());
        CHECK(result.valueType.value() == "HELLO");
    }

    SUBCASE("All uppercase input") {
        Maybe<std::string> input{"HELLO"};
        auto result = str_toupper(input.valueType.value());
        REQUIRE(result.valueType.has_value());
        CHECK(result.valueType.value() == "HELLO");
    }

    SUBCASE("All lowercase input") {
        Maybe<std::string> input{"hello"};
        auto result = str_toupper(input.valueType.value());
        REQUIRE(result.valueType.has_value());
        CHECK(result.valueType.value() == "HELLO");
    }
}

TEST_CASE("Filter One Char Functions") {
    CHECK(filterOneChar(std::string("A")));
    CHECK_FALSE(filterOneChar(std::string("a")));
    CHECK(filterOneChar(std::string("I")));
    CHECK_FALSE(filterOneChar(std::string("i")));
    CHECK_FALSE(filterOneChar(std::string("EPILOGUE")));
}

