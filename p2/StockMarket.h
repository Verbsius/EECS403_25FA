#ifndef STOCKMARKET_H
#define STOCKMARKET_H

#include <string>
#include <queue>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <limits>

using namespace std;

struct Order {
    string type;        // Buy or Sell
    int traderID;       // trader ID
    int stockID;        // Stock Id
    int price;          // Price per share
    int quantity;       // Number of shares
    int timestamp;      // Order arrival time
    int sequence;      

    Order(string t, int td, int s, int p, int q, int ts)
        : type(t), traderID(td), stockID(s), price(p), quantity(q), timestamp(ts) {}
};

// Here are custom comparators
// THe highest-price buyers will be ranked first
struct BuyComparator {
    bool operator()(const Order& a, const Order& b) const {

        // first compare price, then timestamp, then sequence number
        if (a.price != b.price) return a.price < b.price;
        if (a.timestamp != b.timestamp) return a.timestamp > b.timestamp;
        return a.sequence > b.sequence; 
    }
};

// The lowest-price seller will be ranked first
struct SellComparator {
    bool operator()(const Order& a, const Order& b) const {
        if(a.price != b.price) return a.price > b.price;
        if (a.timestamp != b.timestamp) return a.timestamp > b.timestamp;
        return a.sequence > b.sequence;
    }
};

struct Stock {
    int stockID;
    priority_queue<Order, vector<Order>, BuyComparator> buyOrders;
    priority_queue<Order, vector<Order>, SellComparator> sellOrders;

    // Median tracking
    priority_queue<int> lower; // max heap
    priority_queue<int, vector<int>, greater<int>> upper; // min heap


    // Time traveler tracking
    int bestSellTime = -1;
    int bestSellPrice = -1;
    int bestBuyTime = -1;
    int bestBuyPrice = numeric_limits<int>::max();

    
    void addTraderPrice(int price);
    int getMedian() const;
    void updateTimeTravelerBuy(int time, int price);
    void updateTimeTravelerSell(int time, int price);
    void addOrder(const Order& order);

};

struct Trader {
    int traderID;
    int bought = 0;
    int sold = 0;
    long long netTransfer = 0; // positive = gained money, negative = spent
};

class StockMarket {
    private:

        int currentTime = -1; // The current timestamp being processed
        int tradesCompleted = 0; // The number of trades completed

        vector<Stock> stocks; // All stocks in the market
        vector<Trader> traders; // All traders in the market
        
        bool helpFlag = false; // Whether to print help info
        bool verboseFlag = false; // Whether to print verbose info
        bool medianFlag = false; // Whether to print median prices
        bool traderInfoFlag = false; // Whether to print trader info
        bool timeTravelersFlag = false; // Whether to print time travelers
        void processOrders(istream& in);
        void printMedian() const;
        void printSummary() const;
        void printTraderInfo() const;
        void printTimeTravelers() const;
        
        
    
    public:
        void setHelpFlag(bool val) {helpFlag = val;}
        void setVerboseFlag(bool val) {verboseFlag = val;}
        void setMedianFlag(bool val) {medianFlag = val;}
        void setTraderInfoFlag(bool val) {traderInfoFlag = val;}
        void setTimeTraversFlag(bool val) {timeTravelersFlag = val;}
        

        void process(istream& in);

        void printResults();
};


#endif 