// Project Identifier: 0E04A31E0D60C01986ACB20081C9D8722A2519B6
#include <getopt.h>
#include <iostream> 
#include "StockMarket.h"
using namespace std;


void printHelp() {
    cout << "Usage: ./market [OPTIONS] < infile > outfile\n"
         << "Simulates a stock market from order input.\n\n"
         << "Options:\n"
         << "  -h, --help            Show this help message and exit\n"
         << "  -v, --verbose         Print verbose output of trades\n"
         << "  -m, --median          Print median trade prices by stock at each timestamp\n"
         << "  -i, --trader_info     Print trader information summary\n"
         << "  -t, --time_travelers  Print time traveler analysis for each stock\n";
}

int main(int argc, char* argv[]) {

    StockMarket st;
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"verbose", no_argument, 0, 'v'},
        {"median", no_argument, 0, 'm'},
        {"trader_info", no_argument, 0, 'i'},
        {"time_travelers", no_argument, 0, 't'},
        {0, 0, 0, 0}
    };
    int opt;
    while((opt = getopt_long(argc, argv, "hvmit", long_options, nullptr)) != -1) {
        switch(opt) {
            case 'h':
                printHelp();
                return 0;
            case 'v':
                st.setVerboseFlag(true);
                break;
            case 'm':
                st.setMedianFlag(true);
                break;
            case 'i':
                st.setTraderInfoFlag(true);
                break;
            case 't':
                st.setTimeTraversFlag(true);
                break;
            default:
                printHelp();
                return 1;
        }
    }
    
    
    st.process(cin);    

    return 0;
}

