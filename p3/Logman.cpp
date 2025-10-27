#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "Logman.h"


int64_t timestampToInt64(const std::string& ts) {
    int64_t val = 0;
    for (char c : ts) {
        if (c != ':') {
            val = val * 10 + (c - '0');
        }
    }
    return val;
}

std::string timestampToString(int64_t val) {
    std::string digits = std::to_string(val);
    // Ensure leading zeros if needed (since mm, dd, hh, mm, ss are always two digits)
    digits.insert(digits.begin(), 10 - digits.size(), '0');

    // Reinsert colons: mm:dd:hh:mm:ss
    return digits.substr(0, 2) + ":" + digits.substr(2, 2) + ":" +
           digits.substr(4, 2) + ":" + digits.substr(6, 2) + ":" +
           digits.substr(8, 2);
}

// Comparator for LogEntry, ascending based on timestamp. For the tie case, ascending based on entryID
struct LogEntryComparator {
    bool operator()(const LogEntry& a, const LogEntry& b) const {
        if (a.timestamp != b.timestamp) {
            return a.timestamp < b.timestamp;
        }
        return a.entryID < b.entryID;
    }
};

bool Logman::loadMasterLog(const std::string& filename) {
    std::ifstream infile(filename);
    if (!infile.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        exit(1);
    }

    std::string line;
    int id = 0;
    while (getline(infile, line)) {
        
        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);

        int64_t timestamp = timestampToInt64(line.substr(0, p1));
        std::string category = line.substr(p1 + 1, p2 - p1 - 1);
        std::string message = line.substr(p2 + 1);

        
        masterLog.emplace_back(id++, timestamp, category, message);
    }
    // Sort the master log based on timestamp
    std::sort(masterLog.begin(), masterLog.end(), LogEntryComparator());

    
    std::cout << masterLog.size() << " entries read" << std::endl;
}

void Logman::run() {
    std::string line;
    while (true) {
        std::cout << "%";
        if (!std::getline(std::cin, line)) {
            break;
        }

        // skip space
        size_t start = 0;
        while (start < line.size() && std::isspace(static_cast<unsigned char>(line[start]))) {
            start++;
        }
        if (start == line.size()) {
            continue;
        }

        char cmd = line[start];
        while (start < line.size() && std::isspace(static_cast<unsigned char>(line[start]))) {
            start++;
        }
        std::string args;

        args = line.substr(start);



        switch(cmd) {
            case 't':
                timestampsSearch(args);
            case 'm':
                matchingSearch(args);
            case 'c':
                categorySearch(args);
            case 'k':
                keywordSearch(args);
            case 'a':
            case 'r':
            case 'd':
            case 'b':
            case 'e':
            case 's':
            case 'l':
            case 'g':
            case 'p':
            case 'q':
            case '#':
        }
    }
}

void Logman::timestampsSearch(std::string args){
    // get args
    size_t sep = args.find('|');
    if (sep == std::string::npos) {
        std::cerr << "Error: Invalid format. Expected <timestamp1>|<timestamp2>\n";
        return;
    }

    std::string first = args.substr(0, sep);
    std::string second = args.substr(sep + 1);

    int64_t firstInt = timestampToInt64(first);
    int64_t secondInt = timestampToInt64(second);
    
    auto lower = std::lower_bound(masterLog.begin(), masterLog.end(), firstInt, LogEntryComparator());
    auto upper = std::lower_bound(masterLog.begin(), masterLog.end(), secondInt, LogEntryComparator());
    
    size_t cnt = static_cast<size_t>(std::distance(lower, upper));

    // add to lastSearchResults 


}