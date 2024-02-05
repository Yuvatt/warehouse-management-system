CXX = g++
CXXFLAGS = -g -Wall -Weffc++

SRC_DIR = src
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,bin/%.o,$(SRCS))

TARGET = main

all: clean $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o bin/$(TARGET) $^

bin/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f bin/*

PHONY: all clean
