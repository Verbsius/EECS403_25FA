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