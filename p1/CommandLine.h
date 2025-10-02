// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68251FC9
// CommandLine.h
#pragma once
#include <string>

enum class SearchMode { NONE, STACK, QUEUE };

struct CmdOptions {
    SearchMode mode = SearchMode::NONE; // STACK or QUEUE
    bool opt_change = false;            // -c / --change
    bool opt_length = false;            // -l / --length (insert/delete)
    bool opt_swap = false;              // -p / --swap
    char output = 'W';                  // 'W' (word) default or 'M' (modification)
    std::string begin_word;             // -b / --begin (required)
    std::string end_word;               // -e / --end (required)
    bool show_help = false;             // -h / --help
};

class CommandLine {
public:
    // parse command line arguments
    static CommandLine parse(int argc, char** argv); 

    // get parsed options
    const CmdOptions& options() const { return opts_; }

private:
    CmdOptions opts_;
    CommandLine() = default; 
    static void usage_and_exit(int exit_code);  
};
