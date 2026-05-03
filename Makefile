CXX ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -g

.PHONY: all clean help

all: build/cpplings

build/cpplings: src/main.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) src/main.cpp -o build/cpplings

clean:
	rm -rf build .cpplings

help:
	@echo "Targets:"
	@echo "  make       Build the cpplings runner"
	@echo "  make clean Remove local build artifacts"
