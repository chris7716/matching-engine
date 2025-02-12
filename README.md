# Order Matching Engine

## Description
This C++ program simulates a simple order matching engine for a trading system. It allows the addition of buy and sell orders with specific prices and quantities, and it matches them based on the price conditions. The engine will attempt to match the highest buy orders with the lowest sell orders, executing trades when appropriate.

### Key Features:
- **Buy Orders**: Orders that seek to purchase at a specific price or better.
- **Sell Orders**: Orders that seek to sell at a specific price or better.
- **Order Matching**: The engine automatically matches orders when a new one is added, based on the price conditions.
- **Trade Responses**: The matching engine will return responses for the trades.

## Instructions

### Prerequisites
Ensure you have a C++ compiler that supports the C++11 standard or later. Some common compilers include:
- [GCC](https://gcc.gnu.org/)
- [Clang](https://clang.llvm.org/)
- [MSVC](https://developer.microsoft.com/en-us/visualstudio/)

### Steps to Run the Program

1. **Clone or Download the Repository**:
   ```bash
   git clone <repository-url>
   ```
2. **Navigate to the Project Directory: Go to the directory where the main.cpp file is located.**:
    ```bash
   git clone <repository-url>
   ```
3. **Create build folder**
    ```bash
   mkdir build
   ```
4. **Build the project**: This will generate a binary named **matching_engine**
    ```bash
   make
   ```
5. **Run the binary**
    ```bash
   ./matching_engine
   ```
6. **In a separate terminal run the order_generator.py script to submit orders**: This will submit orders to the matching engine application and will generate trades.log file with the responses received from the matching engine.
    ```bash
   python order_generator.py
   ```

Matching engine accepts orders with the following format:
    ```bash
    {order_id},{order_type},{price},{quantity}
    ```
### Example
    ```bash
    1,Buy,100.00,10
    ```
    ```bash
    1,Sell,100.00,10
    ```

### Testing
Tests are available in the **/tests** directory

1. Generate test_engine binary
   ```bash
    g++ -std=c++14 -pthread tests/MatchingEngineTest.cpp \
    src/MatchingEngine.cpp src/TcpServer.cpp \
    -lgtest -lgtest_main -lgmock -lgmock_main -o test_engine
    ```
2. Run the test binary