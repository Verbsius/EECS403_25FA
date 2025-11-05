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

    // Store the most recent search results
    std::vector<size_t> lastSearchReuslt;

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
    void printMostRecentSearchResults();
    void printExcerptList();

    
public:

    Logman() = default;
    
    // Read Master Log and store 
    bool loadFile(const std::string& filename);

    void run();

    

};

#endif // LOGMAN_H