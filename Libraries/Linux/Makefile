CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -fPIC
DEBUG_FLAGS = -g

SRC_DIR = .
BUILD_DIR = build
LIB_DIR = lib

all: debug

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(LIB_DIR)/libNumber.a $(LIB_DIR)/libVector.so $(BUILD_DIR)/testing

$(LIB_DIR)/libNumber.a: $(BUILD_DIR)/NumberLibrary.o
	@mkdir -p $(LIB_DIR)
	ar rcs $@ $^

$(BUILD_DIR)/NumberLibrary.o: $(SRC_DIR)/NumberLibrary.cpp $(SRC_DIR)/NumberLibrary.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(LIB_DIR)/libVector.so: $(BUILD_DIR)/VectorLibrary.o $(LIB_DIR)/libNumber.a
	@mkdir -p $(LIB_DIR)
	$(CXX) -shared -Wl,--whole-archive $(LIB_DIR)/libNumber.a -Wl,--no-whole-archive $< -o $@

$(BUILD_DIR)/VectorLibrary.o: $(SRC_DIR)/VectorLibrary.cpp $(SRC_DIR)/VectorLibrary.h $(SRC_DIR)/NumberLibrary.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/testing: $(BUILD_DIR)/Testing.o $(LIB_DIR)/libVector.so
	$(CXX) $< -L$(LIB_DIR) -lVector -Wl,-rpath,'$$ORIGIN/../lib' -o $@

$(BUILD_DIR)/Testing.o: $(SRC_DIR)/Testing.cpp $(SRC_DIR)/VectorLibrary.h
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(LIB_DIR)

run: $(BUILD_DIR)/testing
	LD_LIBRARY_PATH=$(LIB_DIR):$$LD_LIBRARY_PATH $(BUILD_DIR)/testing

.PHONY: all debug clean run
