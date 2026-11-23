CXX ?= c++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -pedantic -O2
CPPFLAGS += -Iinclude
SOURCES := src/main.cpp src/Circuito.cpp src/parser.cpp
HEADERS := $(wildcard include/*.h)

.PHONY: all test clean
all: build/Parser_Spice
build/Parser_Spice: $(SOURCES) $(HEADERS)
	mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(SOURCES) -o $@
test: build/Parser_Spice
	python3 tests/test_cli.py ./build/Parser_Spice
clean:
	rm -rf build
