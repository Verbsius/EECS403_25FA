// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68251FC9
// CommandLine.cpp
#include "CommandLine.h"
#include <getopt.h>
#include <iostream>
#include <cstdlib>
#include <cctype>

void CommandLine::usage_and_exit(int exit_code) {
    std::cerr <<
    "Usage: letter (--stack | --queue) --begin WORD --end WORD [options]\n"
    "Options:\n"
    "  -h, --help            Show this help message and exit\n"
    "  -s, --stack           Use stack (DFS) routing (exactly one of -s/-q required)\n"
    "  -q, --queue           Use queue (BFS) routing (exactly one of -s/-q required)\n"
    "  -b, --begin WORD      Begin word (required)\n"
    "  -e, --end WORD        End word (required)\n"
    "  -o, --output (W|M)    Output mode: W (words) or M (modifications). Default W\n"
    "  -c, --change          Allow single-letter changes\n"
    "  -l, --length          Allow insert/delete of single letters\n"
    "  -p, --swap            Allow adjacent-letter swaps\n";
    std::exit(exit_code);
}

CommandLine CommandLine::parse(int argc, char** argv) {
    CommandLine cl;
    CmdOptions &opts = cl.opts_;

    static struct option longopts[] = {
        {"help",    no_argument,       nullptr, 'h'},
        {"stack",   no_argument,       nullptr, 's'},
        {"queue",   no_argument,       nullptr, 'q'},
        {"begin",   required_argument, nullptr, 'b'},
        {"end",     required_argument, nullptr, 'e'},
        {"output",  required_argument, nullptr, 'o'},
        {"change",  no_argument,       nullptr, 'c'},
        {"length",  no_argument,       nullptr, 'l'},
        {"swap",    no_argument,       nullptr, 'p'},
        {nullptr,0,nullptr,0}
    };

    int ch;
    opterr = 0; 
    while ((ch = getopt_long(argc, argv, "hsqb:e:o:clp", longopts, nullptr)) != -1) {
        switch (ch) {
            case 'h':
                opts.show_help = true;
                usage_and_exit(0);
                break;
            case 's':
                if (opts.mode != SearchMode::NONE && opts.mode != SearchMode::STACK) {
                    std::cerr << "Error: specify exactly one of --stack/-s or --queue/-q\n";
                    usage_and_exit(1);
                }
                opts.mode = SearchMode::STACK;
                break;
            case 'q':
                if (opts.mode != SearchMode::NONE && opts.mode != SearchMode::QUEUE) {
                    std::cerr << "Error: specify exactly one of --stack/-s or --queue/-q\n";
                    usage_and_exit(1);
                }
                opts.mode = SearchMode::QUEUE;
                break;
            case 'b':
                opts.begin_word = optarg;
                break;
            case 'e':
                opts.end_word = optarg;
                break;
            case 'o':
                opts.output = optarg[0];
                break;
            case 'c':
                opts.opt_change = true;
                break;
            case 'l':
                opts.opt_length = true;
                break;
            case 'p':
                opts.opt_swap = true;
                break;
            case '?':
            default:
                std::cerr << "Error: unknown or malformed option\n";
                usage_and_exit(1);
                break;
        }
    }

    // search mode must be specified
    if (opts.mode == SearchMode::NONE) {
        std::cerr << "Error: must specify exactly one of --stack/-s or --queue/-q\n";
        exit(1);
    }


    // at least one of -c/-l/-p
    if (!opts.opt_change && !opts.opt_length && !opts.opt_swap) {
        std::cerr << "Error: must specify at least one of --change/-c, --length/-l or --swap/-p\n";
        exit(1);
    }

    // begin and end words must be specified
    if (opts.begin_word.empty() || opts.end_word.empty()) {
        std::cerr << "Error: must specify both begin and end words\n";
        exit(1);
    }

    // output mode must be W or M
    if (opts.output != 'W' && opts.output != 'M') {
        std::cerr << "Error: invalid output format\n";
        exit(1);
    }

    // if -l not specified, begin and end words must have same length
    if (!opts.opt_length && opts.begin_word.size() != opts.end_word.size()) {
        std::cerr << "Error: begin/end word lengths mismatch without -l option\n";
        usage_and_exit(1);
    }
    return cl;
}
