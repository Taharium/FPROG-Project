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
        RBTree<std::string> tree = inserted(RBTree<std::string>())(test.begin(), test.end());

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

TEST_CASE("Test insertIntoSet function") {
    
    SUBCASE("simple input") {
        std::string text = "apple banana orange";
        auto result = insertIntoSet(text);
        
        REQUIRE(result.size() == 3);
        REQUIRE(result.count("APPLE") == 1);
        REQUIRE(result.count("BANANA") == 1);
        REQUIRE(result.count("ORANGE") == 1);
    }

    SUBCASE("duplicates") {
        std::string text = "apple banana apple orange banana";
        auto result = insertIntoSet(text);
        
        REQUIRE(result.size() == 3);
        REQUIRE(result.count("APPLE") == 1);
        REQUIRE(result.count("BANANA") == 1);
        REQUIRE(result.count("ORANGE") == 1);
    }

    SUBCASE("case insensitivity") {
        std::string text = "apple Banana orange apple BANANA";
        auto result = insertIntoSet(text);
        
        REQUIRE(result.size() == 3);
        REQUIRE(result.count("APPLE") == 1);
        REQUIRE(result.count("BANANA") == 1);
        REQUIRE(result.count("ORANGE") == 1);
    }

    SUBCASE("empty input") {
        std::string text = "";
        auto result = insertIntoSet(text);
        
        REQUIRE(result.size() == 0);
    }

    SUBCASE("single word input") {
        std::string text = "apple";
        auto result = insertIntoSet(text);
        
        REQUIRE(result.size() == 1);
        REQUIRE(result.count("APPLE") == 1);
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

/** 
 * 
 *  ---------------------------------------- TREE TESTS -----------------------------------------
 * 
 **/



TEST_CASE("Test Node creation") {
    SUBCASE("Test single node creation") {
        RBTree<int> root = std::make_shared<Node<int>>(Color::B, nullptr, 10, nullptr);

        CHECK(root->_val == 10);
        CHECK(root->_c == Color::B);
        CHECK(root->_lft == nullptr);
        CHECK(root->_rgt == nullptr);
    }

    SUBCASE("Test isEmpty function with a non-empty tree") {
        RBTree<int> root = std::make_shared<Node<int>>(Color::B, nullptr, 10, nullptr);
        CHECK_FALSE(isEmpty(root));
    }

    SUBCASE("Test isEmpty function with an empty tree") {
        RBTree<int> emptyTree = nullptr;
        CHECK(isEmpty(emptyTree)); 
    }
}

TEST_CASE("Test rootColor function") {
    SUBCASE("Test rootColor with Black root") {
        RBTree<int> root = std::make_shared<Node<int>>(Color::B, nullptr, 10, nullptr);

        CHECK(rootColor(root) == Color::B);
    }

    SUBCASE("Test rootColor with Red root") {
        RBTree<int> root = std::make_shared<Node<int>>(Color::R, nullptr, 10, nullptr);

        CHECK(rootColor(root) == Color::R);
    }

    SUBCASE("Test rootColor with non-empty tree") {
        RBTree<int> leftChild = std::make_shared<Node<int>>(Color::R, nullptr, 5, nullptr);
        RBTree<int> rightChild = std::make_shared<Node<int>>(Color::R, nullptr, 15, nullptr);
        
        RBTree<int> root = std::make_shared<Node<int>>(Color::B, leftChild, 10, rightChild);

        CHECK(rootColor(root) == Color::B);
    }
}

TEST_CASE("Test root, left, and right functions") {
    
    RBTree<int> leftChild = std::make_shared<Node<int>>(Color::R, nullptr, 5, nullptr);
    RBTree<int> rightChild = std::make_shared<Node<int>>(Color::R, nullptr, 15, nullptr);
    RBTree<int> rootNode = std::make_shared<Node<int>>(Color::B, leftChild, 10, rightChild);

    SUBCASE("Test root function") {
        CHECK(root(rootNode) == 10); 
    }

    SUBCASE("Test left function") {
        CHECK(left(rootNode) == leftChild);
        CHECK(left(rootNode) != rightChild);
    }

    SUBCASE("Test right function") {
        CHECK(right(rootNode) == rightChild);
        CHECK(right(rootNode) != leftChild);
    }
}

TEST_CASE("Test doubledLeft and doubledRight functions") {
    
    RBTree<int> leftChild = std::make_shared<Node<int>>(Color::R, nullptr, 5, nullptr);
    RBTree<int> rightChild = std::make_shared<Node<int>>(Color::R, nullptr, 15, nullptr);
    RBTree<int> blackNode = std::make_shared<Node<int>>(Color::B, nullptr, 20, nullptr);

    RBTree<int> rootNode = std::make_shared<Node<int>>(Color::R, leftChild, 10, rightChild);

    SUBCASE("Test doubledLeft function") {
        CHECK(doubledLeft(rootNode) == true);

        RBTree<int> blackLeftChild = std::make_shared<Node<int>>(Color::B, nullptr, 5, nullptr);
        RBTree<int> rootWithBlackLeft = std::make_shared<Node<int>>(Color::R, blackLeftChild, 10, rightChild);
        CHECK(doubledLeft(rootWithBlackLeft) == false);
    }

    SUBCASE("Test doubledRight function") {
        CHECK(doubledRight(rootNode) == true);

        RBTree<int> blackRightChild = std::make_shared<Node<int>>(Color::B, nullptr, 15, nullptr);
        RBTree<int> rootWithBlackRight = std::make_shared<Node<int>>(Color::R, leftChild, 10, blackRightChild);
        CHECK(doubledRight(rootWithBlackRight) == false);
    }

    SUBCASE("Test doubledLeft with empty tree") {
        RBTree<int> emptyTree = nullptr;
        CHECK(doubledLeft(emptyTree) == false);
    }

    SUBCASE("Test doubledRight with empty tree") {
        RBTree<int> emptyTree = nullptr;
        CHECK(doubledRight(emptyTree) == false);
    }
}

TEST_CASE("Test paint and balance functions") {

    RBTree<int> leftChild = std::make_shared<Node<int>>(Color::R, nullptr, 5, nullptr);
    RBTree<int> rightChild = std::make_shared<Node<int>>(Color::R, nullptr, 15, nullptr);
    RBTree<int> blackChild = std::make_shared<Node<int>>(Color::B, nullptr, 20, nullptr);

    RBTree<int> rootNode = std::make_shared<Node<int>>(Color::R, leftChild, 10, rightChild);

    SUBCASE("Test paint function") {
        auto paintedNode = paintRed<int>(rootNode);
        CHECK(paintedNode->_c == Color::R);
        CHECK(paintedNode->_val == 10);
    }

    SUBCASE("Test balance function with doubledLeft") {
        RBTree<int> balancedNode = balance<int>(Color::B)(rootNode)(20)(rightChild);
        CHECK(balancedNode->_c == Color::R);
    }

    SUBCASE("Test balance function with doubledRight") {
        RBTree<int> balancedNode = balance<int>(Color::B)(rootNode)(20)(rightChild);
        CHECK(balancedNode->_c == Color::R);
    }
}

TEST_CASE("Test Insertion") {
    SUBCASE("Insertion into an empty tree") {
        RBTree<int> emptyTree;
        auto newTree = ins(emptyTree)(42);

        REQUIRE(!isEmpty(newTree));
        CHECK(root(newTree) == 42);
        CHECK(rootColor(newTree) == R);
        CHECK(isEmpty(left(newTree)));
        CHECK(isEmpty(right(newTree)));
    }

    SUBCASE("Insertion into a single-node tree") {
        RBTree<int> singleNodeTree = std::make_shared<const Node<int>>(B, RBTree<int>(), 10, RBTree<int>());
        auto newTree = ins(singleNodeTree)(5);

        REQUIRE(!isEmpty(newTree));
        CHECK(root(newTree) == 10);
        CHECK(rootColor(newTree) == B);
        CHECK(!isEmpty(left(newTree)));
        CHECK(isEmpty(right(newTree)));
        CHECK(root(left(newTree)) == 5);
        CHECK(rootColor(left(newTree)) == R);
    }

    SUBCASE("Prevent duplicate insertion") {
        RBTree<int> singleNodeTree = std::make_shared<const Node<int>>(B, RBTree<int>(), 10, RBTree<int>());
        auto newTree = ins(singleNodeTree)(10);

        CHECK(newTree == singleNodeTree);
    }
}

TEST_CASE("Test insert function") {
    SUBCASE("root normalization") {
        RBTree<int> tree;
        tree = insert(tree)(20);
        tree = insert(tree)(15);
        tree = insert(tree)(25);
        tree = insert(tree)(10);
        tree = insert(tree)(5);

        REQUIRE(!isEmpty(tree));
        CHECK(rootColor(tree) == B);
        CHECK(root(tree) == 15);
    }
}

TEST_CASE("Test inserted function") {
    SUBCASE("Empty range produces the same tree") {
        RBTree<int> emptyTree;
        std::vector<int> elements;

        auto resultTree = inserted(emptyTree)(elements.begin(), elements.end());

        CHECK(resultTree == emptyTree);
    }

    SUBCASE("Insert a single element from a range") {
        RBTree<int> emptyTree;
        std::vector<int> elements = {42};

        auto resultTree = inserted(emptyTree)(elements.begin(), elements.end());

        REQUIRE(!isEmpty<int>(resultTree));
        CHECK(root<int>(resultTree) == 42);
        CHECK(rootColor<int>(resultTree) == B);
        CHECK(isEmpty<int>(left<int>(resultTree)));
        CHECK(isEmpty<int>(right<int>(resultTree)));
    }

}
