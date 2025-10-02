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

void Stock::addOrder(const Order& order) {
    
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
    tradesCompleted = 0;

    stringstream orderStream;
    if(mode_val == "PR") {
        unsigned int seed, num_orders, arrival_rate;
        in >> discard >> seed; 
        in >> discard >> num_orders; 
        in >> discard >> arrival_rate; 

        P2random::PR_init(orderStream, seed, num_traders, num_stocks, num_orders, arrival_rate);
        processOrders(orderStream);
    } else {
        processOrders(in);
    }

    
}

void StockMarket::processOrders(istream& in) {
    int currentTime = -1;
    while (true) {
        int timestamp, traderID, stockID, price, quantity;
        string type, traderStr, stockStr, priceStr, qtyStr;

        if (!(in >> timestamp >> type >> traderStr >> stockStr >> priceStr >> qtyStr)) {
            break; 
        }
        traderID = stoi(traderStr.substr(1));
        stockID = stoi(stockStr.substr(1));
        price = stoi(priceStr.substr(1));
        quantity = stoi(qtyStr.substr(1));

        // TODO 
        if (timestamp != currentTime) {
            if (medianFlag) {
                for (size_t i = 0; i < stocks.size(); i++) {
                    int medianPrice = stocks[i].getMedian();
                    if (medianPrice != -1) {
                        cout << "Median match price of Stock " << i << " at time " << 
                            timestamp << " is $" << medianPrice << "\n";
                    }
                }
            }
            currentTime = timestamp;
        }

        Order order{type, traderID, stockID, price, quantity, timestamp};

        stocks[stockID].addOrder(order);
        

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