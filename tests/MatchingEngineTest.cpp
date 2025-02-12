#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../src/MatchingEngine.h"

// Mock server class to avoid actual socket communication
class MockServer {
public:
    MOCK_METHOD(void, start, (), ());
};

// Mock MatchingEngine class to intercept sendTradeResponse calls
class MockMatchingEngine : public MatchingEngine {
public:
    MockMatchingEngine(int port) : MatchingEngine(port) {}

    MOCK_METHOD(void, sendTradeResponse, (int clientSocket, const std::string& message), (override));
};

// Test 1: Process Order - Valid Order
TEST(MatchingEngineTest, ProcessOrder_ValidOrder) {
    MockMatchingEngine engine(8080);
    EXPECT_CALL(engine, sendTradeResponse(testing::_, testing::_)).Times(1); // Ensure sendTradeResponse is called

    std::string orderMessage = "order1,Buy,100.0,10";
    engine.processOrder(1, orderMessage);  // simulate client socket 1 sending an order

    // Use the getter methods to check the buyOrders queue
    auto buyOrders = engine.getBuyOrders();
    ASSERT_EQ(buyOrders.size(), 1);

    // Access the top element directly (since it's a const reference)
    const Order& order = buyOrders.top();
    EXPECT_EQ(order.id, "order1");
    EXPECT_EQ(order.type, OrderType::Buy);
    EXPECT_EQ(order.price, 100.0);
    EXPECT_EQ(order.quantity, 10);
}

// Test 2: Matching Orders - Trade Execution
TEST(MatchingEngineTest, MatchOrders_TradeExecuted) {
    MockMatchingEngine engine(8080);
    
    // Prepare buy and sell orders
    Order buyOrder("order1", OrderType::Buy, 100.0, 10, 1);
    Order sellOrder("order2", OrderType::Sell, 90.0, 5, 2);

    engine.addBuyOrder(buyOrder);
    engine.addSellOrder(sellOrder);

    // Capture the trade response sent to both clients
    EXPECT_CALL(engine, sendTradeResponse(1, testing::_)).Times(1);
    EXPECT_CALL(engine, sendTradeResponse(2, testing::_)).Times(1);

    engine.matchOrders();

    // Ensure the trade happened
    EXPECT_EQ(engine.getBuyOrders().size(), 1);  // buyOrder should still be in the queue
    EXPECT_EQ(engine.getSellOrders().size(), 0); // sellOrder should be removed after matching
}

// Test 3: No Match - Orders Not Executed
TEST(MatchingEngineTest, MatchOrders_NoMatch) {
    MockMatchingEngine engine(8080);
    
    // Prepare buy and sell orders with incompatible prices
    Order buyOrder("order1", OrderType::Buy, 80.0, 10, 1);
    Order sellOrder("order2", OrderType::Sell, 90.0, 5, 2);

    engine.addBuyOrder(buyOrder);
    engine.addSellOrder(sellOrder);

    EXPECT_CALL(engine, sendTradeResponse(testing::_, testing::_)).Times(0); // No trade response should be sent

    engine.matchOrders();

    // Ensure that no orders were matched
    EXPECT_EQ(engine.getBuyOrders().size(), 1);
    EXPECT_EQ(engine.getSellOrders().size(), 1);
}

// Test 4: Matching Multiple Orders
TEST(MatchingEngineTest, MatchOrders_MultipleOrders) {
    MockMatchingEngine engine(8080);
    
    // Prepare multiple orders
    Order buyOrder1("order1", OrderType::Buy, 100.0, 10, 1);
    Order sellOrder1("order2", OrderType::Sell, 90.0, 5, 2);
    Order buyOrder2("order3", OrderType::Buy, 95.0, 15, 3);
    Order sellOrder2("order4", OrderType::Sell, 95.0, 10, 4);

    engine.addBuyOrder(buyOrder1);
    engine.addBuyOrder(buyOrder2);
    engine.addSellOrder(sellOrder1);
    engine.addSellOrder(sellOrder2);

    // Capture trade responses
    EXPECT_CALL(engine, sendTradeResponse(1, testing::_)).Times(1);
    EXPECT_CALL(engine, sendTradeResponse(2, testing::_)).Times(1);
    EXPECT_CALL(engine, sendTradeResponse(3, testing::_)).Times(1);
    EXPECT_CALL(engine, sendTradeResponse(4, testing::_)).Times(1);

    engine.matchOrders();

    // Ensure that orders were matched properly
    EXPECT_EQ(engine.getBuyOrders().size(), 1);  // One buy order should be left
    EXPECT_EQ(engine.getSellOrders().size(), 1); // One sell order should be left
}

// Test 5: Send Trade Response - Socket Validity
TEST(MatchingEngineTest, SendTradeResponse_ValidSocket) {
    MockMatchingEngine engine(8080);
    
    EXPECT_CALL(engine, sendTradeResponse(1, testing::_)).Times(1);  // sendTradeResponse should be called

    std::string message = "Trade executed: 5 units at 90.0";
    engine.sendTradeResponse(1, message);  // Valid socket
    
    EXPECT_CALL(engine, sendTradeResponse(-1, testing::_)).Times(0);  // No response for invalid socket
    engine.sendTradeResponse(-1, message);  // Invalid socket
}

// Test 6: Process Multiple Orders
TEST(MatchingEngineTest, ProcessMultipleOrders) {
    MockMatchingEngine engine(8080);

    // Prepare multiple orders
    std::string order1 = "order1,Buy,100.0,10";
    std::string order2 = "order2,Sell,90.0,5";
    std::string order3 = "order3,Buy,95.0,20";
    std::string order4 = "order4,Sell,90.0,15";

    // Capture trade responses
    EXPECT_CALL(engine, sendTradeResponse(testing::_, testing::_)).Times(4);

    engine.processOrder(1, order1);
    engine.processOrder(2, order2);
    engine.processOrder(3, order3);
    engine.processOrder(4, order4);

    // Check that the buy and sell queues are updated correctly
    EXPECT_EQ(engine.getBuyOrders().size(), 1); // One buy order left
    EXPECT_EQ(engine.getSellOrders().size(), 1); // One sell order left
}

// Test 7: Process Order - Invalid Message Format
TEST(MatchingEngineTest, ProcessOrder_InvalidFormat) {
    MockMatchingEngine engine(8080);

    std::string invalidOrderMessage = "invalid_order_message";
    
    // Expect sendTradeResponse to be called even with invalid input (edge case handling)
    EXPECT_CALL(engine, sendTradeResponse(testing::_, testing::_)).Times(1);

    engine.processOrder(1, invalidOrderMessage);

    // Ensure that the queues are empty since the order was not processed correctly
    EXPECT_EQ(engine.getBuyOrders().size(), 0);
    EXPECT_EQ(engine.getSellOrders().size(), 0);
}

// Test 8: No Orders to Match
TEST(MatchingEngineTest, MatchOrders_NoOrders) {
    MockMatchingEngine engine(8080);

    // Ensure no orders to match
    engine.matchOrders();

    // Ensure that there are no trades executed
    EXPECT_EQ(engine.getBuyOrders().size(), 0);
    EXPECT_EQ(engine.getSellOrders().size(), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
