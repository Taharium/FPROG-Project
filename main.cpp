#include <iostream>
#include <string>
#include <ranges>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>

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

auto filterBeforeChapter1 = [](const auto& str) -> Maybe<std::string> {
    auto pos = str.find("CHAPTER 1");
    if (pos != std::string::npos) {
        return {str.substr(pos + std::string_view("CHAPTER 1").length())};
    }
    return {std::string{}};
};

auto filterAfterTheEnd = [](const auto& str) -> Maybe<std::string> {
    auto pos = str.find("*** END OF THE PROJECT GUTENBERG EBOOK, WAR AND PEACE ***");
    if (pos != std::string::npos) {
        return {str.substr(0, pos)};
    }
    return {str};
};

auto isAlpha = [](const auto& character) {
    return isalpha(character);
};

auto isPunct = [](const auto& character) {
    return ispunct(character);
};


auto processSpecialChars = [](const auto& word) {
    std::string result;
    std::ranges::for_each(std::ranges::views::iota(0, (int)word.size()), [&word, &result](int i) {
        if(isAlpha(word[i])) {
            result += word[i];
        }
        else if(word[i] == '\'' && isAlpha(word[i - 1]) && isAlpha(word[i + 1])) {
            result += word[i];
        }
        else if(word[i] == '-' && isAlpha(word[i - 1]) && isAlpha(word[i + 1])) {
            result += word[i];
        } else {
            result += ' ';
        }
    });
    // std::copy_if(word.begin(), word.end(), std::back_inserter(result), [&word](char c) {
    //     return std::isalpha(c);
    // });
    return result;
};


// auto isWord = [](const std::string& word) {
//     return !word.empty() && std::all_of(word.begin(), word.end(), isAlpha);
// };

auto getFileContent = [](const auto& path) -> Maybe<std::string>
{
    std::ifstream file(path);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return {content};
};


int main() {
    // std::string text = R"(Some random introduction text. 
    // Chapter 1
    // This is the content of the first chapter. It goes on and on.
    // The End
    // Some irrelevant epilogue text.)";

    //auto content = getFileContent("test.txt");

    auto filePath = "test.txt";

    std::string text = getFileContent(filePath).apply(filterBeforeChapter1).apply(filterAfterTheEnd).valueType.value_or("");
    
    std::unordered_set<std::string> nonfilteredwords;
    // // std::string filteredTextbeginning = filterBeforeChapter1(content);
    // // std::string filteredText = filterAfterTheEnd(filteredTextbeginning);

    std::istringstream stream(text);
    while(stream) {
        std::string word;
        stream >> word;
        nonfilteredwords.insert(word);
    }

    

    auto filteredwords = nonfilteredwords 
                        | std::ranges::views::transform(processSpecialChars);
                        // | std::ranges::views::filter(isWord);

    std::unordered_set<std::string> filteredwords2(filteredwords.begin(), filteredwords.end());

    std::ofstream file("output.txt");
    
    while(file.is_open()) {
        for(const auto& word : filteredwords)
            file << word << " ";
        file.close();
    }


    
    //std::cout << content << std::endl;


    //Apply the filters sequentially


    // std::cout << "Filtered Text:\n" << filteredText << '\n';

    return 0;
}