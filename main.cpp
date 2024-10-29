#include <iostream>
#include "range/v3/all.hpp"
#include <fstream>
#include <unordered_map>
#include <sstream>


int main(){
    std::ifstream file("war_and_peace.txt");
    if(!file.is_open()){
        std::cerr << "ERROR\n";
    }
    std::vector<std::string> lines;
    std::string line;
    int count = 0;
    while(count < 3){
        std::getline(file, line);
        lines.emplace_back(line.c_str());
        /* std::stringstream ss(line);
        std::string word;
        while (ss >> word) {
            lines.push_back(word);
        }
        count++; */
    }
    for (size_t i = 0; i < lines.size(); i++) {
        std::cout << lines[i] << '\n';
    }
}