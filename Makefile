CXX = g++
CXXFLAGS = -std=c++17 -pthread
SRC_DIR = src
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

matching_engine: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o matching_engine

clean:
	rm -rf $(BUILD_DIR) matching_engine
