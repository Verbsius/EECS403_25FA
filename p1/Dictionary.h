// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68251FC9
// Dictionary.h
#pragma once
#include <string>
#include <vector>
#include <algorithm>

class Dictionary {
public:
    // Load words from a file, one word per line
    void loadFromStdin(std::size_t beginLen, bool allowLenChange);

    bool contains(const std::string &word) const {
        return std::find(dictWords.begin(), dictWords.end(), word) != dictWords.end();
    }

    // Get the list of words in the dictionary
    const std::vector<std::string>& words() const { return dictWords; }

private:
    std::vector<std::string> dictWords;

    // Expand a line with possible comma-separated words and insert them into the dictionary
    std::vector<std::string> expandAndInsert(const std::string& line);
};
