// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68251FC9
#include "CommandLine.h"
#include "Dictionary.h"
#include "WordMorpher.h"
#include "Output.h"
#include <iostream>
#include <algorithm>

int main(int argc, char** argv) {

    CommandLine cl = CommandLine::parse(argc, argv);
    const CmdOptions &opts = cl.options();

    // if begin_word = end_word, print it and return immediately
    if (opts.begin_word == opts.end_word) {
        std::cout << "Words in morph: " << 1 << "\n";
        std::cout << opts.begin_word << "\n";
        return 0;
    }

    Dictionary dict;
    dict.loadFromStdin(opts.begin_word.size(), opts.opt_length);


    // Check if both begin_word and end_word are in the dictionary
    if (!dict.contains(opts.begin_word) || !dict.contains(opts.end_word)) {
        std::cerr << "Error: begin or end word not in dictionary\n";
        exit(1);
    }


    WordMorpher morpher(dict, opts);
    auto path = morpher.search();
    
   
    size_t discoveredCount = morpher.discoveredCount();

    Output::print(path, opts, discoveredCount);


    return 0;
}
