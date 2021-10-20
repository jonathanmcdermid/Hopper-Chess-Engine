BIN_NAME=Hopper.exe
BUILD_DIR=build
SRC_DIR=src

CXXFLAGS=-std=c++17

SRC_FILES = $(shell find $(SRC_DIR) -name *.cpp)
OBJ_FILES = $(SRC_FILES:%.cpp=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/$(BIN_NAME) : $(OBJ_FILES)
	$(CXX) $(OBJ_FILES) -o $@

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean

clean:
	@rm -rf $(BUILD_DIR)