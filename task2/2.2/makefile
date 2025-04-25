
CXX = g++
CXXFLAGS = -Wall -Wextra -fopenmp

SRC = task2.cpp
TARGET = task2

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(TARGET)

.PHONY: all clean