// Project Identifier: 50EB44D3F029ED934858FFFCEAC3547C68251FC9
#include "Output.h"

void Output::print(const std::vector<Step> &path,
                   const CmdOptions &opts,
                   size_t discoveredCount) {
    if (path.empty()) {

        std::cout << "No solution, " << discoveredCount << " words discovered.\n";
        return;
    }

    std::cout << "Words in morph: " << path.size() << "\n";

    if (opts.output == 'W') {
        // Word mode
        for (const Step &s : path) {
            std::cout << s.word << "\n";
        }
    } else { 
        // Modification mode
        
        std::cout << path[0].word << "\n";

        for (size_t i = 1; i < path.size(); ++i) {
            const Step &s = path[i];
            switch (s.modType) {
                case 'c':
                    std::cout << "c," << s.modPos << "," << s.modLetter << "\n";
                    break;
                case 'i':
                    std::cout << "i," << s.modPos << "," << s.modLetter << "\n";
                    break;
                case 'd':
                    std::cout << "d," << s.modPos << "\n";
                    break;
                case 's':
                    std::cout << "s," << s.modPos << "\n";
                    break;
                default:
                    break;
            }
        }
    }
}
