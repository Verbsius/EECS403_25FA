// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68251FC9
// WordMorpher.h
#pragma once
#include <string>
#include <vector>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include "CommandLine.h" 
#include "Dictionary.h"

// word: original word
// parentIndex: index of the parent word in steps_ vector
// modType: c (change), i (insert), d (delete), s (swap)
// modPos: position starting from 0
// modLetter: the letter that was modified 
struct Step {
    std::string word;    
    int parentIndex;     
    char modType;        
    int modPos;          
    char modLetter;      
};

class WordMorpher {
public:
    
    WordMorpher(const Dictionary &dict, const CmdOptions &opts);
    std::vector<Step> search();
    size_t discoveredCount() const { return discovered_.size(); }

private:
    const Dictionary &dict_;
    const CmdOptions &opts_;
    std::vector<Step> steps_;   
    std::unordered_set<std::string> discovered_; 

    bool canTransform(const std::string &from,
                      const std::string &to,
                      Step &outStep) const;

};
