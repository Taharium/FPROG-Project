#include <iostream>
#include <ranges>
#include <ranges>
#include "RBTree5.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <optional>
#include <chrono>
#include <algorithm>


int main() {
    // Generate a large dataset of strings
    std::vector<std::string> data;
    for (int i = 0; i < 600000; ++i) {
        data.push_back("String_" + std::to_string(i));
    }

    // Create an RBTree using multithreaded insertion
    RBTree<std::string> tree = insertedParallel<std::string, std::vector<std::string>::iterator, std::vector<std::string>::iterator>(data.begin(), data.end(), 1000);

    // Print the first 10 elements in sorted order
    size_t count = 0;
    forEach(tree, [&](const std::string& val) {
        if (count < 10) {
            std::cout << val << std::endl;
            ++count;
        }
    });

    return 0;
}
