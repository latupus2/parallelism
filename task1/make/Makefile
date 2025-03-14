CXX = g++
CXXFLAGS = -std=c++14 -O3

USE_DOUBLE ?= 0

ifeq ($(USE_DOUBLE), 1)
    CXXFLAGS += -DUSE_DOUBLE
endif

TARGET = SinSum

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.cpp

clean:
	rm -f $(TARGET)