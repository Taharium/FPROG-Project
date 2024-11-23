#include <iostream>
#include <fstream>
#include <string>
#include <optional>
#include <cctype>
#include "range/v3/all.hpp"
#include <utility>
#include <sstream>
#include <unordered_map>


template<typename T>
struct Maybe {
    const std::optional<T> valueType;

    template<typename Func>
    auto apply(const Func& func) -> decltype(func(valueType.value())) {
        if (!valueType.has_value()) {
            return { std::nullopt };
        }
        return func(valueType.value());
    }
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


auto trimText = [](const std::string& text) -> Maybe<std::string> {
    const std::string start_marker = "CHAPTER 1";
    const std::string end_marker = "*** END OF THE PROJECT GUTENBERG EBOOK, WAR AND PEACE ***";

    // Use the extractText function and handle the result
    const auto start_pos = text.find(start_marker);
    const auto end_pos = text.find(end_marker);

    // Return an empty optional if markers are not found or invalid range
    if (start_pos == std::string::npos || end_pos == std::string::npos || end_pos <= start_pos) {
        return {std::nullopt};
    }

    return {text.substr(start_pos + start_marker.length(), end_pos - start_pos - start_marker.length())};
};

auto isAlpha = [](const auto& character) {
    return isalpha(character);
};


auto filterText = [](const std::string& text) -> Maybe<std::string> {

    std::string result;
    // Iterate over the string using indices to capture the characters in a functional style
    ranges::for_each(ranges::views::iota(0, (int)text.size()), [&](int index) {
        // Only process valid characters
        if (isAlpha(text[index])) {
            result += text[index];
        }
        else if(text[index] == '\'' && isAlpha(text[index - 1]) && isAlpha(text[index + 1])) {
            result += text[index];
        }
        /* else if(text[index] == '-' && isAlpha(text[index - 1]) && isAlpha(text[index + 1])) {
            result += text[index];
        } */ else {
            result += ' ';
        }
    });

    using namespace ranges;

    // Transform the text by applying conditions
    /* auto transformed = views::iota(0, (int)text.size())  // Generate indices from 0 to text.size()
        | views::transform([&](int index) {
            char c = text[index];
            // Check if it's an alphabetic character
            if (isAlpha(c)) {
                return c;
            }
            // Check for valid apostrophes (') between alphabets
            else if (c == '\'' && index > 0 && index < text.size() - 1 && isAlpha(text[index - 1]) && isAlpha(text[index + 1])) {
                return c;
            }
            // Check for valid dashes (-) between alphabets
            else if (c == '-' && index > 0 && index < text.size() - 1 && isAlpha(text[index - 1]) && isAlpha(text[index + 1])) {
                return c;
            }
            // Replace other characters with spaces
            else {
                return ' ';
            }
        }); */
     auto transformed = text
        | views::transform([&](char c) {
            // Check if it's an alphabetic character
            if (isAlpha(c)) {
                return c;
            }
            // Check for valid apostrophes (') between alphabets
            else if (c == '\'' && &c > &text.front() && &c < &text.back() &&
                     isAlpha(*(&c - 1)) && isAlpha(*(&c + 1))) {
                return c;
            }
            // Check for valid dashes (-) between alphabets
            else if (c == '-' && &c > &text.front() && &c < &text.back() &&
                     isAlpha(*(&c - 1)) && isAlpha(*(&c + 1))) {
                return c;
            }
            // Replace other characters with spaces
            else {
                return ' ';
            }
        });
        // Only collect the result into a new string when we're done processing
        

    // Convert the transformed view into a std::string
    return {std::string(transformed.begin(), transformed.end())};
};

int main() {
    std::ofstream newFile("output.txt");

    //newFile << readFileIntoString("war_and_peace.txt").apply(trimText).valueType.value();

    std::string text = "HELLO";
    newFile << ranges::views::c_str(text.c_str());
    return 0;
}