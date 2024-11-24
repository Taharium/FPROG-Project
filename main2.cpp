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

auto treeToVector = [](const auto& tree){
    std::vector<std::string> result;
    result.reserve(20000);
    forEach(tree, [&](auto v) {
        result.emplace_back(v);
    });
    return result;
};

void outPut(const auto& result) {
    std::ofstream file("output.txt");
    
    if(file.is_open()){
        std::for_each(result.begin(), result.end(), [&](const auto& str){
            file << str << '\n';
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

auto filterVector = [](const auto& input) {
    std::vector<std::string> result;
    std::copy_if(input.begin(), input.end(), std::back_inserter(result), filterOneChar);
    return result;
};

int main() {
    using namespace std::ranges;
    auto start = std::chrono::high_resolution_clock::now();
    std::string text = readFileIntoString("war_and_peace.txt")
                        .apply(trimText("CHAPTER 1")("*** END OF THE PROJECT GUTENBERG EBOOK, WAR AND PEACE ***"))
                        .apply(filterText).valueType.value_or("");
    
    std::vector<std::string> nonfilteredwords;
    nonfilteredwords.reserve(600000);
    std::cout << nonfilteredwords.max_size();

    std::istringstream stream(text);
    std::string word;
    while(stream) {
        stream >> word;
        nonfilteredwords.emplace_back(str_toupper(word).valueType.value_or(""));      
    }

    auto filteredWords = nonfilteredwords | views::filter(filterOneChar);
    

    auto tree = inserted(RBTree<std::string>(), filteredWords.begin(), filteredWords.end());
    //auto tree = insertedParallel<std::string>(filteredWords.begin(), filteredWords.end(), 1000);
    
    outPut(treeToVector(tree));
    
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Execution time: " << duration.count() << " ms" << std::endl;

    return 0;
}


