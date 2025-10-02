// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68251FC9
// WordMorpher.cpp
#include "WordMorpher.h"
#include <algorithm> 
#include <iostream>


WordMorpher::WordMorpher(const Dictionary &dict, const CmdOptions &opts)
    : dict_(dict), opts_(opts) {
        // Performance Optimization: reserve space for steps_ and discovered_
        steps_.reserve(dict_.words().size());
        discovered_.reserve(dict_.words().size());
    }

std::vector<Step> WordMorpher::search() {

    // Initialize 
    Step start{opts_.begin_word, -1, 0, 0, 0};
    steps_.push_back(start);
    discovered_.insert(opts_.begin_word);

    std::deque<int> container;
    // start from index 0
    container.push_back(0);
    int endIndex = -1;


    // Performance Optimization: cache the words from the dictionary
    const auto & words =  dict_.words();




    while (!container.empty()) {
        int curIdx;
        
        if (opts_.mode == SearchMode::QUEUE) {
            // queue
            curIdx = container.front();
            container.pop_front();
        } else { // stack
            curIdx = container.back();
            container.pop_back();
        }

        // get the current step
        const Step & curStep = steps_[curIdx];
        size_t curLen = curStep.word.size();

        
        for (auto &w : words) {

            // OP: put the hash search in the end
            bool lengthOK = false;

            // change
            if (opts_.opt_change && w.size() == curLen) lengthOK = true;

            // swap
            if (opts_.opt_swap && w.size() == curLen) lengthOK = true;

            // insert/delete
            if (opts_.opt_length && (w.size() == curLen + 1 || w.size() == curLen - 1)) lengthOK = true;

            if (!lengthOK) continue;

            if (discovered_.count(w)) continue;
            


            // found the undiscovered word that can be transformed, 
            Step newStep{w, curIdx, 0, 0, 0};
            if (canTransform(curStep.word, w, newStep)) {
                steps_.push_back(newStep);
                
                int newIdx = static_cast<int>(steps_.size() - 1);
                discovered_.insert(w);
                container.push_back(newIdx);

                if (w == opts_.end_word) {


                    endIndex = newIdx;

                    // found the end word and clear the container to stop searching
                    container.clear();
                    break;
                }
            }
        }
    }

    // No solution
    if (endIndex == -1) {
        return {};
    }

    std::vector<Step> path;
    for (int idx = endIndex; idx != -1; idx = steps_[idx].parentIndex) {
        path.push_back(steps_[idx]);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

// determine if 'from' can be transformed to 'to' based on opts_
// from: original word
// to: target word
// outStep: if can be transformed, fill in the modification info to outStep

bool WordMorpher::canTransform(const std::string &from,
                               const std::string &to, 
                               Step &outStep) const {
    
    const size_t from_len = from.size();
    const size_t to_len = to.size();

    // change
    if (opts_.opt_change && from_len == to_len) {
        int diff_count = 0, diff_pos = -1;
        for (size_t i = 0; i < from_len; ++i) {
            if (from[i] != to[i]) {
                ++diff_count;
                if (diff_count == 1) diff_pos = static_cast<int>(i);
                else break;
            }
        }
        if (diff_count == 1) {
            outStep.modType = 'c';
            outStep.modPos = diff_pos;
            outStep.modLetter = to[diff_pos];
            return true;
        }
    }

    // swap
    if (opts_.opt_swap && from_len == to_len && from_len >= 2) {
        size_t first_diff = 0;
        while (first_diff < from_len && from[first_diff] == to[first_diff]) {
            ++first_diff;
        }
        
        if (first_diff < from_len - 1 &&
            from[first_diff] == to[first_diff + 1] && 
            from[first_diff + 1] == to[first_diff]) {
            
  
            bool match = true;
            for (size_t i = first_diff + 2; i < from_len; ++i) {
                if (from[i] != to[i]) {
                    match = false;
                    break;
                }
            }
            if (match) {
                outStep.modType = 's';
                outStep.modPos = static_cast<int>(first_diff);
                outStep.modLetter = 0;
                return true;
            }
        }
    }

    // insert/delete
    if (opts_.opt_length) {
        if (to_len == from_len + 1) { 
            // insert
            size_t i = 0;
            while (i < from_len && from[i] == to[i]) ++i;
            
      
            bool match = true;
            for (size_t k = 0; match && k < from_len - i; ++k) {
                if (from[i + k] != to[i + 1 + k]) match = false;
            }
            if (match) {
                outStep.modType = 'i';
                outStep.modPos = static_cast<int>(i);
                outStep.modLetter = to[i];
                return true;
            }
        } else if (to_len == from_len - 1) { 
            // delete
            size_t i = 0;
            while (i < to_len && from[i] == to[i]) ++i;
            
            // 检查 from[i+1:] == to[i:]
            bool match = true;
            for (size_t k = 0; match && k < to_len - i; ++k) {
                if (from[i + 1 + k] != to[i + k]) match = false;
            }
            if (match) {
                outStep.modType = 'd';
                outStep.modPos = static_cast<int>(i);
                outStep.modLetter = from[i];
                return true;
            }
        }
    }

    return false;
}