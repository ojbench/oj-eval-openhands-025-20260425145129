CXXFLAGS=-g -O2 -std=c++17
BINS=cheat anticheat
SRCS=lang.cpp

all: code

# Build both cheat and anticheat
cheat: cheat.o lang.o
	$(CXX) $(CXXFLAGS) -o $@ $^

anticheat: anticheat.o lang.o
	$(CXX) $(CXXFLAGS) -o $@ $^

# Build object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Default target: build cheat as 'code'
code: cheat anticheat
	cp cheat code

.PHONY: clean
clean:
	rm -f $(BINS) code *.o

.PHONY: test
test: cheat anticheat
	@echo "Built cheat and anticheat successfully"
