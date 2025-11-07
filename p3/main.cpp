// Project Identifier: 01BD41C3BF016AD7E8B6F837DF18926EC3251350
#include "Logman.h"
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " -f <logfile> OR --help" << std::endl;
        return 1;
    }

    std::string option = argv[1];

    if (option == "-h" || option == "--help") {
        std::cout << "Usage: " << argv[0] << " [OPTION]\n\n"
                  << "Options:\n"
                  << "  -h, --help           Show this help message and exit\n"
                  << "  -f, --file LOGFILE   Specify input log file to load\n\n"
                  << "Example:\n"
                  << "  " << argv[0] << " --file myLogFile.txt\n\n";
        exit(0); 
    }


    if (option == "-f" || option == "--file") {
        if (argc < 3) {
            std::cerr << "Error: missing log file name after " << option << "\n";
            return 1;
        }

        std::string logFileName = argv[2];


        Logman manager;
        manager.loadFile(logFileName);  
        manager.run();

        return 0;
    }

    std::cerr << "Unknown option: " << option << "\n"
              << "Use -h or --help for usage info.\n";
    return 1;

}