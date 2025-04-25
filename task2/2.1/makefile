
CXX = g++
CXXFLAGS = -Wall -Wextra -fopenmp

SRC = task1.cpp
TARGET = task1

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(TARGET)

.PHONY: all clean