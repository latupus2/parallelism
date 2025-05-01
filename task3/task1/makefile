CXX := g++
CXXFLAGS := -std=c++11 -Wall -Wextra -O3 -pthread

SRC := task1.cpp
TARGET := matrix_multiply

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $< -o $@


clean:
	rm -f $(TARGET)

.PHONY: all clean