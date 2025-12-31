CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude

SRC := $(wildcard src/**/*.cpp) $(wildcard src/*.cpp)
OBJ := $(SRC:.cpp=.o)

memsim: $(OBJ)
	$(CXX) $(OBJ) -o memsim.exe

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) memsim.exe
