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
    string type;
    unsigned int timestamp, traderID, stockID, price, quantity;
    unsigned int currentTime = 0;

    while (in >> timestamp >> type) {
        string traderStr, stockStr, priceStr, qtyStr;
        in >> traderStr >> stockStr >> priceStr >> qtyStr;
        traderID = stoi(traderStr.substr(1));
        stockID = stoi(stockStr.substr(1));
        price = stoi(priceStr.substr(1));
        quantity = stoi(qtyStr.substr(1));

        if (timestamp != currentTime) {
            if (medianFlag) {
                    
            }
        }

    }
}