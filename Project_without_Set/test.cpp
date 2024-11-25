#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <iostream>
#include "functions.h"

TEST_CASE("Testing trimText function") {
    auto trim = trimText("start")("end");

    SUBCASE("Valid trim") {
        auto result = trim("This is a start trimmed end text");
        std::string trimmed(" trimmed ");
        CHECK_EQ(result.valueType.value(), trimmed);
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
        std::string input{"hello"};
        auto result = str_toupper(input);
        CHECK(result == "HELLO");
    }

    SUBCASE("Empty string") {
        std::string input{""};
        auto result = str_toupper(input);
        CHECK(result == "");
    }

    SUBCASE("Mixed case input") {
        std::string input{"HeLlO"};
        auto result = str_toupper(input);
        CHECK(result == "HELLO");
    }

    SUBCASE("All uppercase input") {
        std::string input{"HELLO"};
        auto result = str_toupper(input);
        CHECK(result == "HELLO");
    }

    SUBCASE("All lowercase input") {
        std::string input{"hello"};
        auto result = str_toupper(input);
        CHECK(result == "HELLO");
    }
}

TEST_CASE("Filter One Char Functions") {
    CHECK(filterInvalid(std::string("A")));
    CHECK_FALSE(filterInvalid(std::string("a")));
    CHECK(filterInvalid(std::string("I")));
    CHECK_FALSE(filterInvalid(std::string("i")));
    CHECK_FALSE(filterInvalid(std::string("EPILOGUE")));
}

TEST_CASE("Test treeToVector function") {
    SUBCASE("Multiple elements in tree") {
        std::vector<std::string> test = { "b", "a", "c", "d"};
        RBTree<std::string> tree = parallelInsert(RBTree<std::string>())(test.begin(), test.end());

        auto result = treeToVector(tree);

        std::vector<std::string> expected = {"a", "b", "c", "d"};

        CHECK_EQ(result.size(), expected.size());
        CHECK_EQ(result[0], expected[0]);
        CHECK_EQ(result[1], expected[1]);
        CHECK_EQ(result[2], expected[2]);
        CHECK_EQ(result[3], expected[3]);
    }

    SUBCASE("Empty tree") {
        RBTree<std::string> emptyTree;
        auto result = treeToVector(emptyTree);
        std::vector<std::string> expected;
        CHECK(result == expected);
    }

    SUBCASE("Single element tree") {
        RBTree<std::string> tree = insert(RBTree<std::string>())("10"); 
        auto result = treeToVector(tree);
        std::vector<std::string> expected = {"10"};
        CHECK(result == expected);
    }
}

TEST_CASE("Test outPut function") {
    std::stringstream result;
    result << "Hello, world!";

    const char* filePath = "test_output.txt";
    auto writeToFile = outPut(result);
    writeToFile(filePath);

    std::ifstream file(filePath);
    std::string fileContent;
    file.seekg(0, std::ios::end);
    fileContent.resize(file.tellg());
    file.seekg(0, std::ios::beg); 
    file.read(&fileContent[0], fileContent.size());

    CHECK(fileContent == "Hello, world!");
    std::remove(filePath);
}

DOCTEST_TEST_CASE("Test outPut with empty stringstream") {
    std::stringstream result;

    const char* filePath = "empty_test_output.txt";
    auto writeToFile = outPut(result);
    writeToFile(filePath);

    std::ifstream file(filePath);
    std::string fileContent;
    file.seekg(0, std::ios::end);
    fileContent.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(&fileContent[0], fileContent.size());

    CHECK(fileContent.empty());
    std::remove(filePath);
}

TEST_CASE("Test readFileIntoString function") {
    
    SUBCASE("Non-existent file") {
        const char* filename = "non_existent_file.txt";
        
        auto result = readFileIntoString(filename);
        
        CHECK(!result.valueType.has_value()); 
        std::remove(filename);
    }

    SUBCASE("Empty file") {
        const char* filename = "empty_file.txt";
        
        std::ofstream file(filename);
        
        auto result = readFileIntoString(filename);
        
        CHECK(result.valueType.has_value());
        CHECK(result.valueType.value() == "");
        std::remove(filename);

    }
}

TEST_CASE("Test insertIntoVector function") {
    
    SUBCASE("simple input") {
        std::string text = "apple banana orange";
        auto result = insertIntoVector(text);
        
        CHECK(result.size() == 3);
        CHECK(result[0] == "APPLE");
        CHECK(result[1] == "BANANA");
        CHECK(result[2] == "ORANGE");
    }

    // Input with duplicate words
    SUBCASE("duplicates") {
        std::string text = "apple banana apple orange banana";
        auto result = insertIntoVector(text);
        
        CHECK(result.size() == 5);
        CHECK(result[0] == "APPLE");
        CHECK(result[1] == "BANANA");
        CHECK(result[2] == "APPLE");
        CHECK(result[3] == "ORANGE");
        CHECK(result[4] == "BANANA");
    }

    SUBCASE("case insensitivity") {
        std::string text = "apple Banana orange apple BANANA";
        auto result = insertIntoVector(text);
        
        CHECK(result.size() == 5);
        CHECK(result[0] == "APPLE");
        CHECK(result[1] == "BANANA");
        CHECK(result[2] == "ORANGE");
        CHECK(result[3] == "APPLE");
        CHECK(result[4] == "BANANA");
    }

    SUBCASE("empty input") {
        std::string text = "";
        auto result = insertIntoVector(text);
        
        CHECK(result.size() == 0);
    }

    SUBCASE("single word input") {
        std::string text = "apple";
        auto result = insertIntoVector(text);
        
        CHECK(result.size() == 1);
        CHECK(result[0] == "APPLE");
    }
}

TEST_CASE("Test insertIntoStream function") {

    SUBCASE("simple input") {
        std::vector<std::string> words = {"apple", "banana", "orange"};
        auto resultStream = insertIntoStream(words);

        std::string line;
        std::getline(resultStream, line);
        CHECK(line == "apple");

        std::getline(resultStream, line);
        CHECK(line == "banana");

        std::getline(resultStream, line);
        CHECK(line == "orange");

        CHECK_FALSE(std::getline(resultStream, line));
    }

    SUBCASE("empty input") {
        std::vector<std::string> words = {};
        auto resultStream = insertIntoStream(words);

        std::string line;
        CHECK_FALSE(std::getline(resultStream, line));
    }

    SUBCASE("empty strings as words") {
        std::vector<std::string> words = {"apple", "", "orange"};
        auto resultStream = insertIntoStream(words);

        std::string line;
        std::getline(resultStream, line);
        CHECK(line == "apple");

        std::getline(resultStream, line);
        CHECK(line == "");

        std::getline(resultStream, line);
        CHECK(line == "orange");

        CHECK_FALSE(std::getline(resultStream, line));
    }

    SUBCASE("single word input") {
        std::vector<std::string> words = {"apple"};
        auto resultStream = insertIntoStream(words);

        std::string line;
        std::getline(resultStream, line);
        CHECK(line == "apple");

        CHECK_FALSE(std::getline(resultStream, line));
    }
}