#ifndef MATCHING_ENGINE_H
#define MATCHING_ENGINE_H

#include <iostream>
#include <queue>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <string>
#include "TCPServer.h"

enum class OrderType { Buy, Sell };

struct Order {
    std::string id;
    OrderType type;
    double price;
    int quantity;
    int clientSocket;

    Order(std::string _id, OrderType _type, double _price, int _quantity, int _clientSocket)
        : id(std::move(_id)), type(_type), price(_price), quantity(_quantity), clientSocket(_clientSocket) {}

    // Define comparison operator for priority queue
    bool operator<(const Order& other) const {
        if (price == other.price) {
            return id > other.id; // If prices are equal, prioritize older orders
        }
        return (type == OrderType::Buy) ? (price < other.price) : (price > other.price);
    }
};

class MatchingEngine {
public:
    MatchingEngine(int port);
    void processOrder(int clientSocket, const std::string& orderMessage);

    const std::priority_queue<Order>& getBuyOrders() const { return buyOrders; }
    const std::priority_queue<Order>& getSellOrders() const { return sellOrders; }
    void matchOrders();

    virtual void sendTradeResponse(int clientSocket, const std::string& message);
    virtual void sendTradeToClient(int clientSocket, const std::string& tradeMessage);

    void addBuyOrder(const Order& order) {
        buyOrders.push(order);
    }

    void addSellOrder(const Order& order) {
        sellOrders.push(order);
    }
    
private:
    std::priority_queue<Order> buyOrders;
    std::priority_queue<Order> sellOrders;
    std::mutex orderBookMutex;
    TcpServer server;
};

#endif
