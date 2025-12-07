// Project Identifier: 1761414855B69983BD8035097EFBD312EB2517F0

#include <iomanip>
#include <iostream>
#include <getopt.h>
#include "Drone.h"


void printHelp() {
    std::cout << "Usage: drone -m {MST|FASTTSP|OPTTSP}\n";
}

int main(int argc, char** argv) {
    std::cout << std::setprecision(2); //Always show 2 decimal places
    std::cout << std::fixed; //Disable scientific notation for large numbers

    std::string mode = "";
    const char* const short_opts = "m:h";
    const option long_opts[] = {
        {"mode", required_argument, nullptr, 'm'},
        {"help", no_argument, nullptr, 'h'},
        {nullptr, no_argument, nullptr, 0}
    };


    while (true) {
        const auto opt = getopt_long(argc, argv, short_opts, long_opts, nullptr);
        if (opt == -1) break;
        switch (opt) {
            case 'm':
                mode = std::string(optarg);
                break;
            case 'h':
                printHelp();
                break;
            default:
                std::cerr << "Unknown option. Use -h for help.\n";
                exit(1);
        }
    }
    Drone drone;
    drone.load();

    if (mode == "MST") {
        if (!drone.constructMST()) {
            std::cerr << "Cannot construct MST\n";
        }
        drone.outputMST();
    } else if (mode == "FASTTSP") {
        drone.computeFASTTSP();
        drone.outputFASTTSP();
        
    } else if (mode == "OPTTSP") {
        drone.computeOPTTSP();
        drone.outputOPTTSP();
    }


}