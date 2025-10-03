// Project Identifier: 0E04A31E0D60C01986ACB20081C9D8722A2519B6
#include <iostream>
#include <sstream>
#include "StockMarket.h"
#include "P2random.h"
using namespace std;


// ================================= Stock Methods ===============================

void Stock::addTraderPrice(int price) {
    if (lower.empty() || price <= lower.top()) {
        lower.push(price);
    } else {
        upper.push(price);
    }

    // Balance the heaps
    if (lower.size() > upper.size() + 1) {
        upper.push(lower.top());
        lower.pop();
    } else if (upper.size() > lower.size()) {
        lower.push(upper.top());
        upper.pop();
    }
}
// Return -1 if no prices have been added
int Stock::getMedian() const {
    if (lower.empty()) return -1;
    
    if (lower.size() == upper.size()) {
        return (lower.top() + upper.top()) / 2;
    } else {
        return lower.top();
    }
}

void Stock::updateTimeTravelerBuy(int time, int price) {
    
    if (price < bestBuyPrice) {
        bestBuyPrice = price;
        bestBuyTime = time;
    }
}

void Stock::updateTimeTravelerSell(int time, int price) {
    if (price > bestSellPrice) {
        bestSellPrice = price;
        bestSellTime = time;
    }
}



void Stock::addAndMatchOrder(const Order& order, bool verboseFlag, 
        vector<Trader>& traders, int& tradesCompleted){
    if (order.type == "BUY") {
        buyOrders.push(order);
    }

    if (order.type == "SELL") {
        sellOrders.push(order);
    }

    while (!buyOrders.empty() && !sellOrders.empty()) {

        Order buyTop = buyOrders.top();
        Order sellTop = sellOrders.top();
        if (buyTop.price < sellTop.price) {
            break; // No match possible
        }
        tradesCompleted++;

        int tradePrice = buyTop.sequence < sellTop.sequence ? buyTop.price : sellTop.price;

        int tradeQuantity = min(buyTop.quantity, sellTop.quantity);

        if (verboseFlag) {
            cout << "Trader " << buyTop.traderID << " purchased " << tradeQuantity << 
                " shares of Stock " << buyTop.stockID << " from Trader " << sellTop.traderID <<
                " for $" << tradePrice << "/share\n";
        }

        buyTop.quantity -= tradeQuantity;
        sellTop.quantity -= tradeQuantity;
        buyOrders.pop();
        sellOrders.pop();
        if (buyTop.quantity > 0) buyOrders.push(buyTop);
        if (sellTop.quantity > 0) sellOrders.push(sellTop);

        // Medain related
        addTraderPrice(tradePrice);

        // Time traveler related
        updateTimeTravelerBuy(buyTop.timestamp, tradePrice);
        updateTimeTravelerSell(sellTop.timestamp, tradePrice);

        // Trader related
        long long totalCost = static_cast<long long>(tradePrice) * tradeQuantity;
        
        traders[buyTop.traderID].bought += tradeQuantity;
        traders[buyTop.traderID].netTransfer -= totalCost;
        traders[sellTop.traderID].sold += tradeQuantity;
        traders[sellTop.traderID].netTransfer += totalCost;
        

        
    }
}

// ================================= StockMarket Methods ===============================
void StockMarket::process(istream& in) {
    cout << "Processing orders...\n";

    // Read the first four lines of input 

    string discard;
    getline(in, discard);
    string mode_val;
    int num_traders, num_stocks;

    in >> discard >> mode_val; 
    in >> discard >> num_traders; 
    in >> discard >> num_stocks; 

    
    // Initialize stocks and traders
    stocks.clear();
    stocks.reserve(num_stocks);
    traders.clear();
    traders.reserve(num_traders);

    stringstream orderStream;
    if(mode_val == "PR") {
        unsigned int seed, num_orders, arrival_rate;
        in >> discard >> seed; 
        in >> discard >> num_orders; 
        in >> discard >> arrival_rate; 

        P2random::PR_init(orderStream, seed, num_traders, num_stocks, num_orders, arrival_rate);
        processOrders(orderStream);
    } 

    if (mode_val == "TL") {
        processOrders(in);
    }

    
}

void StockMarket::processOrders(istream& in) {
    while (true) {
        int timestamp, traderID, stockID, price, quantity;
        string type, traderStr, stockStr, priceStr, qtyStr;

        if (!(in >> timestamp >> type >> traderStr >> stockStr >> priceStr >> qtyStr)) {
            break; 
        }

        if (timestamp < 0) {
            cerr << "Error: Negative timestamp\n";
            exit(1);
        }

        if (timestamp < currentTime) {
            cerr << "Error: Decreasing timestamp\n";
            exit(1);
        }

        traderID = stoi(traderStr.substr(1));
        if (traderID < 0 || traderID >= static_cast<int>(traders.size())) {
            cerr << "Error: Invalid trader ID\n";
            exit(1);
        }

        stockID = stoi(stockStr.substr(1));
        if (stockID < 0 || stockID >= static_cast<int>(stocks.size())) {
            cerr << "Error: Invalid stock ID\n";
            exit(1);
        }

        price = stoi(priceStr.substr(1));
        if (price <= 0) {
            cerr << "Error: Invalid price\n";
            exit(1);
        }

        quantity = stoi(qtyStr.substr(1));
        if (quantity <= 0) {
            cerr << "Error: Invalid quantity\n";
            exit(1);
        }


    
        if (timestamp != currentTime) {
            currentTime = timestamp;
            if (medianFlag) printMedian();
        }

        Order order{type, traderID, stockID, price, quantity, timestamp, sequenceNumber++};
        stocks[stockID].addAndMatchOrder(order, verboseFlag, traders, tradesCompleted);
        
        

    }


    if (medianFlag) printMedian();

    printSummary();

    if (traderInfoFlag) printTraderInfo();
 
    if (timeTravelersFlag) printTimeTravelers();
}


void StockMarket::printMedian() const {
    for (size_t i = 0; i < stocks.size(); i++) {
        int medianPrice = stocks[i].getMedian();
        if (medianPrice != -1) {
            cout << "Median match price of Stock " << i << " at time " << 
                currentTime << " is $" << medianPrice << "\n";
        }
    }
}

void StockMarket::printSummary() const {
    cout << "---End of Day---\n" << 
        "Trades Completed: " << tradesCompleted << "\n";
}

void StockMarket::printTraderInfo() const {
    cout << "---Trader Info---\n";
    for (size_t i = 0; i < traders.size(); i++) {
        cout << "Trader " << i << ": bought " << traders[i].bought << 
            " and sold " << traders[i].sold << 
            " for a net transfer of $" << traders[i].netTransfer << "\n";
    }
}

void StockMarket::printTimeTravelers() const {
    cout << "---Time Travelers---\n";
    for (size_t i = 0; i < stocks.size(); i++) {
        const Stock& curStock = stocks[i];
        if (curStock.bestBuyTime != -1 && curStock.bestSellTime != -1 && 
                curStock.bestBuyTime < curStock.bestSellTime && 
                curStock.bestBuyPrice != numeric_limits<int>::max() &&
                curStock.bestSellPrice != -1 &&
                curStock.bestSellPrice > curStock.bestBuyPrice) {
            cout << "A time traveler would buy Stock " << i << " at time " <<
                curStock.bestBuyTime << " for $" << curStock.bestBuyPrice <<
                " and sell it at time " << curStock.bestSellTime <<" for $" <<
                curStock.bestSellPrice << "\n";
        } else {
            cout << "A time traveler could not make a profit on Stock " << i << "\n";
        }
    }
}