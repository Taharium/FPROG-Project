#include <iostream>
#include <ranges>
#include "RBTree9.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <optional>
#include <chrono>
#include <unordered_set>
#include <string_view>
#include <algorithm>

template<typename T>
struct Maybe {
    const std::optional<T> valueType;

    template<typename Func>
    auto apply(const Func& func) -> decltype(func(valueType.value())) {
        if (!valueType.has_value()) {
            return {std::nullopt};
        }
        return func(valueType.value());
    }
};

auto trimText = [](const std::string& startMarker) {
    return [startMarker](const std::string& endMarker) {
        return [startMarker, endMarker](const auto& text) -> Maybe<std::string_view> {
            const auto start_pos = text.find(startMarker);
            const auto end_pos = text.find(endMarker);

            if (start_pos == std::string::npos || end_pos == std::string::npos || end_pos <= start_pos) {
                return {std::nullopt};
            }

            return {std::string_view{text}.substr(start_pos + startMarker.length(), end_pos - start_pos - startMarker.length())};
        };
    };
};

auto isAlpha = [](const auto& character) {
    return isalpha(character);
};

auto filterText = [](const auto& text) -> Maybe<std::string> {
    using namespace std::ranges;

    auto transformed = views::iota(0, (int)text.size())
        | views::transform([&](int index) {
            char c = text[index];
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

auto outPut = [](const auto& result) {
    return [&result](const char* filePath){
        std::ofstream file(filePath);
        if(!file){
            std::cerr << "\nCould not open file\n";
        }
        file << result.rdbuf();
    };
};

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

auto str_toupper = [](const auto& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(),
                [](unsigned char c) { return std::toupper(c); });
    return result; 
};

auto filterOneChar = [](const auto& word){
    return !((word.size() == 1 && (word != "A" && word != "I")) || word == "EPILOGUE") ;
};

auto insertIntoSet = [](const auto& text){
    std::unordered_set<std::string> nonfilteredwords;
    nonfilteredwords.reserve(600000);

    std::istringstream stream(text);
    std::string word;
    while(stream >> word) {
        nonfilteredwords.insert(str_toupper(word));      
    }
    return nonfilteredwords;
};

auto insertIntoStream = [](const auto& words){
    std::ostringstream oss;
    std::for_each(words.begin(), words.end(), [&oss](const auto& word){
        oss << word << '\n';
    });
    return std::stringstream(oss.str());
};

auto printTime = [](const auto& duration){
    std::cout << "Execution time: " << duration.count() << " ms" << std::endl;
};

int main() {
    using namespace std::ranges;
    auto start = std::chrono::high_resolution_clock::now();

    std::string text = readFileIntoString("war_and_peace.txt")
                        .apply(trimText("CHAPTER 1")("*** END OF THE PROJECT GUTENBERG EBOOK, WAR AND PEACE ***"))
                        .apply(filterText).valueType.value_or("");
    
    auto nonfilteredwords = insertIntoSet(text);

    auto filteredWords = nonfilteredwords | views::filter(filterOneChar);

    auto tree = inserted(RBTree<std::string>()) (filteredWords.begin(), filteredWords.end());
    //auto tree = inserted(RBTree<std::string>(), filteredWords.begin(), filteredWords.end());
    
    outPut(insertIntoStream(treeToVector(tree)))("output.txt");
    
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    printTime(duration);

    return 0;
}

