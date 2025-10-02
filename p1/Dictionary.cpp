// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68251FC9
// Dictionary.cpp
#include "Dictionary.h"
#include <iostream>
#include <algorithm>

void Dictionary::loadFromStdin(std::size_t beginLen, bool allowLenChange) {
    dictWords.clear();
    char type;
    if (!(std::cin >> type)) {
        std::cerr << "Error: missing dictionary type\n";
        exit(1);
    }
    std::size_t numOfLines;
    if (!(std::cin >> numOfLines)) {
        std::cerr << "Error: missing dictionary line count\n";
        exit(1);
    }
    
    std::string line;
    std::getline(std::cin, line); 
    for (std::size_t i = 0; i < numOfLines; ) {
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;
        if (line.rfind("//",0) == 0) continue; 

        
        std::vector<std::string> tempWords;
        
        if (type == 'S') {
            // Simple dictionary
            tempWords.push_back(line);
        } else {
            // Complex dictionary
            tempWords = expandAndInsert(line);
        }


        for (auto & w : tempWords) {
            if (!allowLenChange && w.size() != beginLen) continue;
            dictWords.push_back(std::move(w));
        }
        ++i;


    }

}

std::vector<std::string> Dictionary::expandAndInsert(const std::string& line) {

    std::vector<std::string> result;
    if (line.empty()) return result;
    // reversal
    if (line.back() == '&') {
        std::string word = line.substr(0, line.size()-1);
        result.push_back(word);
        std::string rev = word;
        std::reverse(rev.begin(), rev.end());
        result.push_back(rev);
        return result;
    }

    // insert-each
    auto lb = line.find('[');
    auto rb = line.find(']');
    if (lb != std::string::npos && rb != std::string::npos) {
        std::string prefix = line.substr(0, lb);
        std::string middle = line.substr(lb + 1, rb - lb - 1);
        std::string suffix = line.substr(rb + 1);
        for(char c:middle) {
            result.push_back(prefix + c + suffix); // insert each character
        }
        return result;
    }

    // swap !
    auto ex = line.find('!');
    if (ex != std::string::npos) {
        std::string base = line;
        base.erase(ex, 1); // remove !
        result.push_back(base);
        if (ex >= 2) {
            std::string swapped = base;
            std::swap(swapped[ex-2], swapped[ex-1]);
            result.push_back(swapped);
        }
        return result;
    }


    // double ?
    auto qm = line.find('?');
    if (qm != std::string::npos) {
        std::string base = line;
        base.erase(qm, 1);
        result.push_back(base);
        if (qm >= 1) {
            std::string doubled=base;
            doubled.insert(qm, 1, base[qm-1]);
            result.push_back(doubled);
        }
        return result;
    }
    // no special chars
    result.push_back(line);
    return result;
}


