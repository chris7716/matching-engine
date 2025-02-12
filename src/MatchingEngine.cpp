#include "MatchingEngine.h"
#include <sstream>
#include <unistd.h>

MatchingEngine::MatchingEngine(int port) 
    : server(port, std::bind(&MatchingEngine::processOrder, this, std::placeholders::_1, std::placeholders::_2)) {
    server.start();
}

void MatchingEngine::processOrder(int clientSocket, const std::string& orderMessage) {
    std::lock_guard<std::mutex> lock(orderBookMutex);

    std::istringstream ss(orderMessage);
    std::string orderId, orderTypeStr, priceStr, quantityStr;
    std::getline(ss, orderId, ',');
    std::getline(ss, orderTypeStr, ',');
    std::getline(ss, priceStr, ',');
    std::getline(ss, quantityStr, ',');

    double price = std::stod(priceStr);
    int quantity = std::stoi(quantityStr);
    OrderType orderType = (orderTypeStr == "Buy") ? OrderType::Buy : OrderType::Sell;
    
    Order order(orderId, orderType, price, quantity, clientSocket);
    
    if (order.type == OrderType::Buy) {
        buyOrders.push(order);
    } else {
        sellOrders.push(order);
    }
    
    matchOrders();
    
    std::string response = "Order received: " + order.id + "\n";
    sendTradeResponse(clientSocket, response);
}

void MatchingEngine::sendTradeResponse(int clientSocket, const std::string& message) {
    if (clientSocket != -1) {
        send(clientSocket, message.c_str(), message.length(), 0);
    }
}

void MatchingEngine::sendTradeToClient(int clientSocket, const std::string& tradeMessage) {
    sendTradeResponse(clientSocket, tradeMessage);
}

void MatchingEngine::matchOrders() {
    while (!buyOrders.empty() && !sellOrders.empty()) {
        Order buyOrder = buyOrders.top();
        Order sellOrder = sellOrders.top();

        if (buyOrder.price >= sellOrder.price) {
            int tradeQuantity = std::min(buyOrder.quantity, sellOrder.quantity);
            double tradePrice = sellOrder.price;

            std::string tradeMessage = "Trade executed: " + std::to_string(tradeQuantity) + 
                                       " units at " + std::to_string(tradePrice) + "\n";

            sendTradeToClient(buyOrder.clientSocket, tradeMessage);
            sendTradeToClient(sellOrder.clientSocket, tradeMessage);

            buyOrder.quantity -= tradeQuantity;
            sellOrder.quantity -= tradeQuantity;

            buyOrders.pop();
            sellOrders.pop();

            if (buyOrder.quantity > 0) {
                buyOrders.push(buyOrder);
            }

            if (sellOrder.quantity > 0) {
                sellOrders.push(sellOrder);
            }
        } else {
            break;
        }
    }
}