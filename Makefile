CXX ?= c++
CLANG_FORMAT ?= clang-format
CLANG_TIDY ?= clang-tidy
CPPFLAGS += -Iinclude
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -g

CORE_SRCS := \
	src/cli.cpp \
	src/manifest.cpp \
	src/output.cpp \
	src/progress.cpp \
	src/runner.cpp \
	src/util.cpp \
	src/worktree.cpp

TEST_LIB_SRCS := \
	src/manifest.cpp \
	src/output.cpp \
	src/progress.cpp \
	src/util.cpp \
	src/worktree.cpp

TEST_NAMES := manifest worktree progress output
TEST_BINS := $(addprefix build/test_,$(TEST_NAMES))
HEADERS := $(wildcard include/cpplings/*.hpp)
FORMAT_FILES := $(HEADERS) $(CORE_SRCS) src/main.cpp $(wildcard tests/*.cpp tests/*.hpp)
LINT_FILES := $(CORE_SRCS) src/main.cpp $(wildcard tests/*.cpp)

.PHONY: all clean format format-check help lint test

all: build/cpplings

build/cpplings: src/main.cpp $(CORE_SRCS) $(HEADERS)
	@mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) src/main.cpp $(CORE_SRCS) -o build/cpplings

build/test_%: tests/test_%.cpp tests/test_support.hpp $(TEST_LIB_SRCS) $(HEADERS)
	@mkdir -p build
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $< $(TEST_LIB_SRCS) -o $@

test: $(TEST_BINS)
	@for test in $(TEST_BINS); do \
		echo "Running $$test"; \
		$$test || exit $$?; \
	done

format:
	$(CLANG_FORMAT) -i $(FORMAT_FILES)

format-check:
	$(CLANG_FORMAT) --dry-run --Werror $(FORMAT_FILES)

lint:
	$(CLANG_TIDY) $(LINT_FILES) -- $(CPPFLAGS) $(CXXFLAGS)

clean:
	rm -rf build build-cmake .cpplings

help:
	@echo "Targets:"
	@echo "  make              Build the cpplings runner"
	@echo "  make test         Build and run unit tests"
	@echo "  make format       Format C++ sources"
	@echo "  make format-check Check C++ formatting"
	@echo "  make lint         Run clang-tidy"
	@echo "  make clean        Remove local build artifacts"
