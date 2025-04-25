
CXX = g++
CXXFLAGS = -Wall -Wextra -fopenmp

TARGETS = task3v1 task3v2 task3v3

all: $(TARGETS)

task3v1: task3v1.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

task3v2: task3v2.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

task3v3: task3v3.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)

.PHONY: all clean