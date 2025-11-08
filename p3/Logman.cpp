// Project Identifier: 01BD41C3BF016AD7E8B6F837DF18926EC3251350
// P3 for EECS281
// Author: ee
// Date: 11/5/2025

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include "Logman.h"


int64_t Logman::timestampStringToInt64(const std::string& ts) const {
    int64_t val = 0;
    for (char c : ts) {
        if (c != ':') {
            val = val * 10 + (c - '0');
        }
    }
    return val;
}

std::string Logman::timestampInt64ToString(const int64_t val) const {
    std::string digits = std::to_string(val);
    // Ensure leading zeros if needed (since mm, dd, hh, mm, ss are always two digits)
    digits.insert(digits.begin(), 10 - digits.size(), '0');

    // Reinsert colons: mm:dd:hh:mm:ss
    return digits.substr(0, 2) + ":" + digits.substr(2, 2) + ":" +
           digits.substr(4, 2) + ":" + digits.substr(6, 2) + ":" +
           digits.substr(8, 2);
}

// Comparator for LogEntry, ascending based on timestamp. For the tie case, ascending based on entryID
struct SortedByTimeComparator {
    const std::vector<LogEntry>& masterLogList;

    SortedByTimeComparator(const std::vector<LogEntry>& log) : masterLogList(log){}

    bool operator()(size_t a, size_t b) const {
        const auto& A = masterLogList[a];
        const auto& B = masterLogList[b];
        if (A.timestamp != B.timestamp) {
            return A.timestamp < B.timestamp;
        }
        return A.entryID < B.entryID;
    }
};

std::string Logman::toLowerString(const std::string& in) {
    std::string out;
    out.reserve(in.size());
    for (char c : in) {
        out += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    } 
    return out;
}




bool Logman::load(const std::string& filename) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(1);
    }

    std::string line;
    size_t id = 0;
    while (getline(infile, line)) {
        
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);

        int64_t timestamp = timestampStringToInt64(line.substr(0, p1));
        std::string category = line.substr(p1 + 1, p2 - p1 - 1);
        std::string message = line.substr(p2 + 1);

        // Update masterLogList

        masterLogList.emplace_back(id, timestamp, category, message);

        // Update categoryMap
        std::string catLower = toLowerString(category);
        categoryMap[catLower].emplace_back(id);

        // Update keywordMap
        std::unordered_set<std::string> seenKeywords;

        // Parse keyword from s and store the unique keyword in keywordMap
        auto processText = [&](const std::string& s) {
            std::string token;
            for (char c : s) {
                if (isalnum((unsigned char)c)) {
                    token += c;
                } else {
                    if (!token.empty()) {
                        std::string low = toLowerString(token);
                        if (!seenKeywords.count(low)) {
                            keywordMap[low].push_back(id);
                            seenKeywords.insert(low);
                        }
                        token.clear();
                    }
                }
            }
            // final token
            if (!token.empty()) {
                std::string low = toLowerString(token);
                if (!seenKeywords.count(low)) {
                    keywordMap[low].push_back(id);
                    seenKeywords.insert(low);
                }
            }
        };

        processText(line.substr(p1 + 1));
        
        // Inialize sortedByTimestamp
        sortedByTimestamp.push_back(id);
        id++;
    }

    infile.close();

    // Sort sortedByTimestamp based on timestamp in ascending order
    std::sort(sortedByTimestamp.begin(), sortedByTimestamp.end(), SortedByTimeComparator(masterLogList));
    for (auto& [cat, vec] : categoryMap) {
        std::sort(vec.begin(), vec.end(), SortedByTimeComparator(masterLogList));
    }
    for (auto& [kw, vec] : keywordMap) {
        std::sort(vec.begin(), vec.end(), SortedByTimeComparator(masterLogList));
    }

    
    std::cout << masterLogList.size() << " entries read" << std::endl;

    return true;
}

void Logman::run() {
    std::string line;
    do {
        if (std::cin.fail()) {
            std::cerr << "cin entered fail state: exiting" << std::endl;
            exit(1);
        } 

        std::cout << "% ";
        if (!std::getline(std::cin, line)) {
            std::cerr << "cannot get a line from cin" << std::endl;
            exit(1);
        }
        size_t pos = 0;
        while (pos < line.size() && std::isspace(static_cast<unsigned char>(line[pos]))) {
            ++pos;
        }

        if (pos >= line.size()) {
            continue; 
        }

        char cmd = line[pos];
        if (cmd == '#') {
            continue; 
        }

        ++pos; 

        while (pos < line.size() && std::isspace(static_cast<unsigned char>(line[pos]))) {
            ++pos;
        }

        std::string args;
        if (pos < line.size()) {
            args = line.substr(pos);
        }

        switch (cmd) {
            case 't':
                timestampsSearch(args);
                break;
            case 'm':
                matchingSearch(args);
                break;
            case 'c':
                categorySearch(args);
                break;
            case 'k':
                keywordSearch(args);
                break;
            case 'a':
                appendLogEntry(args);
                break;
            case 'r':
                appendSearchEntry();
                break;
            case 'd':
                deleteLogEntry(args);
                break;
            case 'b':
                moveToBeginning(args);
                break;
            case 'e':
                moveToEnd(args);
                break;
            case 's':
                sortExcerptList();
                break;
            case 'l':
                clearExcerptList();
                break;
            case 'g':
                printLastSearchResults();
                break;
            case 'p':
                printExcerptList();
                break;
            case 'q':
                return; // quit program
            default:
                // ignore unknown commands
                break;
        }

    } while (true); 
}

void Logman::timestampsSearch(std::string args){
    size_t barPos = args.find('|');
    std::string startStr = args.substr(0, barPos);
    std::string endStr = args.substr(barPos + 1);

    int64_t startTimestamp = timestampStringToInt64(startStr);
    int64_t endTimestamp = timestampStringToInt64(endStr);

    lastSearchReuslt.clear();

    auto lower = std::lower_bound(
        sortedByTimestamp.begin(), sortedByTimestamp.end(), startTimestamp,
        [&](size_t idx, int64_t val) {
            return masterLogList[idx].timestamp < val;
        }
    );

    auto upper = std::upper_bound(
        sortedByTimestamp.begin(), sortedByTimestamp.end(), endTimestamp,
        [&](int64_t val, size_t idx) {
            return val < masterLogList[idx].timestamp;
        }
    );

    for (auto it = lower; it != upper; it++) {
        lastSearchReuslt.push_back(*it);
    }

    std::cout << "Timestamps search: " << lastSearchReuslt.size() << " entries found" << std::endl;

}

void Logman::matchingSearch(std::string args){
 

    int64_t targetTime = timestampStringToInt64(args);

    lastSearchReuslt.clear();

    auto lower = std::lower_bound(
        sortedByTimestamp.begin(), sortedByTimestamp.end(), targetTime,
        [&](size_t idx, int64_t val) {
            return masterLogList[idx].timestamp < val;
        }
    );

    auto upper = std::upper_bound(
        sortedByTimestamp.begin(), sortedByTimestamp.end(), targetTime,
        [&](int64_t val, size_t idx) {
            return val < masterLogList[idx].timestamp;
        }
    );

    for (auto it = lower; it != upper; it++) {
        lastSearchReuslt.push_back(*it);
    }

    std::cout << "Timestamps search: " << lastSearchReuslt.size() << " entries found" << std::endl;

}

void Logman::categorySearch(std::string args) {
    std::string catLow = toLowerString(args);
    lastSearchReuslt.clear();
    if (categoryMap.count(catLow)) {
        lastSearchReuslt = categoryMap[catLow];
    }
    std::cout << "Category search: " << lastSearchReuslt.size() << " entries found" << std::endl;

}

void Logman::keywordSearch(std::string args) {
    std::vector<std::string> keywords;
    std::string token;
    for (char c : args) {
        if (isalnum((unsigned char)c)) {
            token += c;
        } else {
            if (!token.empty()) {
                keywords.push_back(toLowerString(token));
                token.clear();
            }
        }
    }
    if (!token.empty()) keywords.push_back(toLowerString(token));

    lastSearchReuslt.clear();
    if (keywords.empty()) return;


    if (!keywordMap.count(keywords[0])) {
        std::cout << "Keyword search: 0 entries found" << std::endl;
        return;
    }


    std::vector<size_t> result = keywordMap[keywords[0]];


    for (size_t i = 1; i < keywords.size(); ++i) {
        auto it = keywordMap.find(keywords[i]);
        if (it == keywordMap.end()) {
            result.clear();
            break;  
        }

        std::vector<size_t> temp;
        std::set_intersection(
            result.begin(), result.end(),
            it->second.begin(), it->second.end(),
            std::back_inserter(temp),
            SortedByTimeComparator(masterLogList)
        );
        result.swap(temp);
        if (result.empty()) break; 
    }

    lastSearchReuslt = std::move(result);
    std::cout << "Keyword search: " << lastSearchReuslt.size() << " entries found" << std::endl;
}

void Logman::appendLogEntry(std::string args) {
    // Delete blank space
    size_t start = args.find_first_not_of(" \t");
    if (start == std::string::npos) return;
    size_t end = args.find_last_not_of(" \t");
    std::string trimmed = args.substr(start, end - start + 1);

    
    size_t entryID = std::stoul(trimmed);


    if (entryID >= masterLogList.size()) {

        std::cerr << "ERROR: invalid entryID" << std::endl;
        return; 
    }

    excerptList.push_back(entryID);

    std::cout << "log entry " << entryID << " appended" << std::endl;
}
void Logman::appendSearchEntry() {
    if (lastSearchReuslt.empty()) {
        return;
    }
    excerptList.insert(excerptList.end(), lastSearchReuslt.begin(), lastSearchReuslt.end());
    std::cout << lastSearchReuslt.size() << " log entries appended" << std::endl;
}
void Logman::deleteLogEntry(std::string args){
    size_t start = args.find_first_not_of(" \t");
    size_t end = args.find_last_not_of(" \t");
    std::string trimmed = args.substr(start, end - start + 1);
    size_t excerptIdx = std::stoul(trimmed);

    excerptList.erase(excerptList.begin() + excerptIdx);
    std::cout << "Deleted excerpt list entry " << excerptIdx << std::endl;
}

void Logman::moveToBeginning(std::string args) {
    size_t start = args.find_first_not_of(" \t");
    size_t end = args.find_last_not_of(" \t");
    std::string trimmed = args.substr(start, end - start + 1);
    size_t excerptIdx = std::stoul(trimmed);

    auto entry = excerptList[excerptIdx];
    excerptList.erase(excerptList.begin() + excerptIdx);
    excerptList.insert(excerptList.begin(), entry);

    std::cout << "Moved excerpt list entry " << excerptIdx << std::endl;


}
void Logman::moveToEnd(std::string args) {
    size_t start = args.find_first_not_of(" \t");
    size_t end = args.find_last_not_of(" \t");
    std::string trimmed = args.substr(start, end - start + 1);
    size_t excerptIdx = std::stoul(trimmed);

    auto entry = excerptList[excerptIdx];
    excerptList.erase(excerptList.begin() + excerptIdx);
    excerptList.push_back(entry);
    std::cout << "Moved excerpt list entry " << excerptIdx << std::endl;
}
void Logman::sortExcerptList() {
    std::cout << "excerpt list sorted" << std::endl;

    if (excerptList.empty()) {
        std::cout << "(previously empty)" << std::endl;
        return;
    }

    std::cout << "previous ordering:" << std::endl;
    printExcerptEntry(0);
    std::cout << "..." << std::endl;
    printExcerptEntry(excerptList.size() - 1);

    std::sort(excerptList.begin(), excerptList.end(), SortedByTimeComparator(masterLogList));

    std::cout << "new ordering:" << std::endl;
    printExcerptEntry(0);
    std::cout << "..." << std::endl;
    printExcerptEntry(excerptList.size() - 1);

}
void Logman::clearExcerptList() {
    std::cout << "excerpt list cleared" << std::endl;

    if (excerptList.empty()) {
        std::cout << "(previously empty)" << std::endl;
        return;
    }

    std::cout << "previous contents:" << std::endl;

    printExcerptEntry(excerptList.front());

    std::cout << "..." << std::endl;
    printExcerptEntry(excerptList.back());

    excerptList.clear();
}

void Logman::printLastSearchResults() {
    if (lastSearchReuslt.empty()) return;
    for (size_t i = 0; i < lastSearchReuslt.size(); i++) {
        printMasterEntry(lastSearchReuslt[i]);
    }

}
void Logman::printExcerptList(){
    if (excerptList.empty()) return;
    for (size_t i = 0; i < excerptList.size(); i++) {
        printExcerptEntry(i);
    }
}

void Logman::printExcerptEntry(size_t id) const {
    if (id >= excerptList.size()) return;
    const auto& entry = masterLogList[excerptList[id]];
    std::cout << id << "|"
              << entry.entryID << "|"
              << timestampInt64ToString(entry.timestamp) << "|"
              << entry.category << "|"
              << entry.message << std::endl;
}
void Logman::printMasterEntry(size_t id) const {
    const auto& entry = masterLogList[id];
    std::cout << entry.entryID << "|"
              << timestampInt64ToString(entry.timestamp) << "|"
              << entry.category << "|"
              << entry.message << std::endl;
}