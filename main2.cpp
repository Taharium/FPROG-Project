#include <iostream>
#include <ranges>
#include <ranges>
#include "RBTree4.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <optional>
#include <chrono>
#include <algorithm>
/* #include <windows.h>

void printMemoryUsage() {
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    
    if (GlobalMemoryStatusEx(&statex)) {
        std::cout << "Memory load: " << statex.dwMemoryLoad << "%\n";
        std::cout << "Total physical memory: " << statex.ullTotalPhys / 1024 / 1024 << " MB\n";
        std::cout << "Available physical memory: " << statex.ullAvailPhys / 1024 / 1024 << " MB\n";
    } else {
        std::cerr << "Error getting memory status\n";
    }
} */
/* std::string result;
    result.reserve(text.size());
    // Iterate over the string using indices to capture the characters in a functional style
    ranges::for_each(ranges::views::iota(0, (int)text.size()), [&](int index) {
        // Only process valid characters
        if (isAlpha(text[index])) {
            result += text[index];
        }
        else if(text[index] == '\'' && isAlpha(text[index - 1]) && isAlpha(text[index + 1])) {
            result += text[index];
        }
        else if(text[index] == '-' && isAlpha(text[index - 1]) && isAlpha(text[index + 1])) {
            result += text[index];
        } else {
            result += ' ';
        }
    });
    return {result}; */

/* void printMemoryUsage() {
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    
    if (GlobalMemoryStatusEx(&statex)) {
        std::cout << "Memory load: " << statex.dwMemoryLoad << "%\n";
        std::cout << "Total physical memory: " << statex.ullTotalPhys / 1024 / 1024 << " MB\n";
        std::cout << "Available physical memory: " << statex.ullAvailPhys / 1024 / 1024 << " MB\n";
    } else {
        std::cerr << "Error getting memory status\n";
    }
} */

template<typename T>
struct Maybe {
    const std::optional<T> valueType;

    template<typename Func>
    auto apply(const Func& func) -> decltype(func(valueType.value())) {
        if (!valueType.has_value()) {
            return {std::nullopt};
        }
        return func(std::move(valueType.value()));
    }
};

auto trimText = [](const std::string& startMarker) {
    return [startMarker](const std::string& endMarker) {
        return [startMarker, endMarker](const std::string& text) -> Maybe<std::string> {
            const auto start_pos = text.find(startMarker);
            const auto end_pos = text.find(endMarker);

            if (start_pos == std::string::npos || end_pos == std::string::npos || end_pos <= start_pos) {
                return {std::nullopt}; // Immutable failure state
            }

            return {text.substr(start_pos + startMarker.length(), end_pos - start_pos - startMarker.length())};
        };
    };
};

auto isAlpha = [](const auto& character) {
    return isalpha(character);
};

auto filterText = [](const auto& text) -> Maybe<std::string> {
    using namespace std::ranges;

    // Transform the text by applying conditions
    auto transformed = views::iota(0, (int)text.size())  // Generate indices from 0 to text.size()
        | views::transform([&](int index) {
            char c = text[index];
            // Check if it's an alphabetic character
            if (isAlpha(c)) {
                return c;
            }
            else if (c == '\'' && index > 0 && index < text.size() - 1 && isAlpha(text[index - 1]) && isAlpha(text[index + 1])) {
                return c;
            }
            else if (c == '-' && index > 0 && index < text.size() - 1 && isAlpha(text[index - 1]) && isAlpha(text[index + 1])) {
                return c;
            }
            else {
                return ' ';
            }
        });
     
        
    return {std::string(transformed.begin(), transformed.end())};
};

template<class T>
void outPut(RBTree<T> const & t) {
    std::ofstream file("output.txt");
    
    if(file.is_open()){
        forEach(t, [&file](T v) {
            file << v << '\n';
        });
    }
}

auto readFileIntoString = [](const auto& filename) -> Maybe<std::string> {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        return {std::nullopt};   
    }
    
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string buffer(size, '\0');

    if(file.read(&buffer[0], size)){
        return {buffer};
    } 
        
    return {std::nullopt};
};

auto str_toupper = [](const auto& s) -> Maybe<std::string> {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return {result}; 
};

auto filterOneChar = [](const auto& word){
    return !((word.size() == 1 && (word != "A" && word != "I")) || word == "EPILOGUE") ;
};
template<typename T>
void test(std::initializer_list<T>){
    return;
}

template <typename T>
std::initializer_list<T> vector_to_initializer_list(const std::vector<T>& vec) {
    // Create a temporary array and return it as an initializer list
    return {vec.begin(), vec.end()};
}

int main() {
    using namespace std::ranges;
    auto start = std::chrono::high_resolution_clock::now();
    std::string text = readFileIntoString("war_and_peace.txt")
                        .apply(trimText("CHAPTER 1")("*** END OF THE PROJECT GUTENBERG EBOOK, WAR AND PEACE ***"))
                        .apply(filterText).valueType.value_or("");
    
    std::vector<std::string> nonfilteredwords;
    nonfilteredwords.reserve(600000);

    std::istringstream stream(text);
    std::string word;
    while(stream) {
        stream >> word;
        nonfilteredwords.emplace_back(str_toupper(word).valueType.value_or(""));      
    }

    auto filteredWords = nonfilteredwords | views::filter(filterOneChar);
    //std::vector<std::string> filtered(filteredWords.begin(), filteredWords.end());
    //std::cout << '\n' << filtered.size() << '\n';
    //auto tree = inserted(RBTree<std::string>(), filtered.begin(), filtered.end());
    //printMemoryUsage();
    /* std::vector<std::string> init = {
        "A", "red", "black", "tree", "walks", "into", "A", "bar",
        "has", "j", "walker", "o", "the", "rocks",
        "and", "quickly", "rebalances", "itself.",
        "A", "RED", "BLACK", "TREE", "WALKS", "INTO", "A", "BAR",
        "HAS", "JOHNNY", "R", "ON", "THE", "ROCKS",
        "AND", "QUICKLY", "R", "ITSELF."
    };
    auto filtered = init | std::ranges::views::filter(filterOneChar); */

    //std::cout << nonfilteredwords.size() << '\n';
    //auto tree = insertedParallel(RBTree<std::string>(), filteredWords.begin(), filteredWords.end(), 1000);
    auto tree = inserted(RBTree<std::string>(), filteredWords.begin(), filteredWords.end());
    //printMemoryUsage();
    //std::sort(filteredWords.begin(), filteredWords.end());
    //std::vector<std::string> words(filtered.begin(), filtered.end());
    //RBTree<std::string> tree (vector_to_initializer_list(words));
    //auto tree = insertedParallel(filteredWords.begin(), filteredWords.end(), 1000)
    //std::cout << tree.countB() << '\n';
    outPut(tree);
    //std::unordered_set<std::string> filteredwords2(filteredText.begin(), filteredText.end());
    
    //std::ofstream file("output.txt");

    
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Execution time: " << duration.count() << " ms" << std::endl;

    return 0;
}


