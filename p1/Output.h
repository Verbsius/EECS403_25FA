// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68251FC9
#pragma once
#include <vector>
#include <iostream>
#include "CommandLine.h"
#include "WordMorpher.h"


class Output {
public:
    static void print(const std::vector<Step> &path,
                      const CmdOptions &opts,
                      size_t discoveredCount);
};
