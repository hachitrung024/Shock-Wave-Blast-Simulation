CXX      = g++
# Base flags common to both modes
CXXFLAGS = -Iinclude -Wall -std=c++17
SRC_DIR  = src
OBJ_DIR  = obj
TARGET   = main.exe

# Check if DEBUG variable is set (e.g., make DEBUG=1)
ifdef DEBUG
    # Debug mode: Add debug symbols (-g) and disable optimization (-O0)
    CXXFLAGS += -g -O0
else
    # Release mode: Enable high optimization (-O3) for best performance
    CXXFLAGS += -O3
endif

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean