#ifndef LOGMAN_H
#define LOGMAN_H

#include <iostream>
#include <string>
#include <vector>


struct LogEntry {
    size_t entryID;
    int64_t timestamp;
    std::string category;
    std::string message;
    
};


class Logman {
private:
    std::vector<LogEntry> masterLog;  // 


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
    bool loadMasterLog(const std::string& filename);

    void run();

    

};

#endif // LOGMAN_H