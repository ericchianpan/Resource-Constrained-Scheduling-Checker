CXX = g++
CXXFLAGS = -O3 -std=c++11
SRC = src/main.cpp src/Results.cpp src/Circuit.cpp
OBJECTS = $(SRC:.cpp=.o)

EXECUTABLE = checker

all: $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(EXECUTABLE)

$(OBJDIS)/.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

clean:
	rm -f src/*.o $(EXECUTABLE)
