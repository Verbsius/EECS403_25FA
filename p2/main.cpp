// Project Identifier: 0E04A31E0D60C01986ACB20081C9D8722A2519B6

#include <iostream> 
#include "StockMarket.h"
using namespace std;
int main() {

    StockMarket st;
    
    // Parse command line arguments
    bool helpFlag = false;
    bool verboseFlag = false;
    bool medianFlag = false;
    bool traderInfoFlag = false;
    bool timeTravelersFlag = false;
    
    st.setHelpFlag(helpFlag);
    st.setVerboseFlag(verboseFlag);
    st.setMedianFlag(medianFlag);
    st.setTraderInfoFlag(traderInfoFlag);
    st.setTimeTraversFlag(timeTravelersFlag);
    
    st.process(cin);


    
    

    return 0;
}