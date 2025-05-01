CXX = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++17 -pthread

TARGET = server_clients
SRC = task2.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)

rebuild: clean all