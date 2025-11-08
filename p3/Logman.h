// Project Identifier: 01BD41C3BF016AD7E8B6F837DF18926EC3251350
// P3 for EECS281
// Author: ee
// Date: 11/5/2025
#ifndef LOGMAN_H
#define LOGMAN_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>


struct LogEntry {
    size_t entryID;
    int64_t timestamp;
    std::string category;
    std::string message;
    
    LogEntry(size_t id, int64_t ts, std::string& c, std::string& m) : entryID(id), timestamp(ts), category(c), message(m) {}
    
};


class Logman {
private:
    // Store the original data in Log when loading
    std::vector<LogEntry> masterLogList;  

    // Store the list of indices in masterLog for user edit commend
    std::vector<size_t> excerptList;

    // Store indices for each category in hash table when loading 
    std::unordered_map<std::string, std::vector<size_t>> categoryMap;

    // Store the index in masterLog corresponding to each keyword when loading
    std::unordered_map<std::string, std::vector<size_t>> keywordMap;
    
    // Store the index of entry sorted by timestamp ascendingly
    std::vector<size_t> sortedByTimestamp; 

    // Point to the most recent search results.  
    std::vector<size_t>* lastSearchResult = nullptr;

    

private:
    void timestampsSearch(std::string args);
    void matchingSearch(std::string args);
    void categorySearch(std::string args);
    void keywordSearch(std::string args);
    void appendLogEntry(std::string args);
    void appendSearchEntry();
    void deleteLogEntry(std::string args);
    void moveToBeginning(std::string args);
    void moveToEnd(std::string args);
    void sortExcerptList();
    void clearExcerptList();
    void printLastSearchResults();
    void printExcerptList();

    // Helper functions
    // Transform in to all lower-case string
    std::string toLowerString(const std::string& in) ;

    // Transform timestamp in string to int64
    int64_t timestampStringToInt64(const std::string& ts) const;

    // Transform timestamp in int64 to string
    std::string timestampInt64ToString(const int64_t val) const;

    // Parse keyword from input and store unique key in keyMap
    void parseKeyword(const std::string& in); 

    // Print a single line of excerpt list
    void printExcerptEntry(size_t id) const;

    // Print a line of master log list
    void printMasterEntry(size_t id) const;
public:

    Logman() = default;
    
    // Open file according to filename load
    bool load(const std::string& filename);

    void run();
    
    ~Logman() {
        delete lastSearchResult;
    }

    

};

#endif // LOGMAN_H