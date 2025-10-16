# # Compiler and flags
# CXX = g++
# CXXFLAGS = -Wall -Wextra -std=c++17 -g
# LDFLAGS = -lncurses

# # Include paths
# INCLUDES = -Iinclude

# # Source files in src/

# SRC = src/main.cpp src/Dungeon.cpp src/Monster.cpp src/Player.cpp src/Room.cpp src/Event.cpp src/SaveLoad.cpp


# # Object files in build/
# OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))

# # Output executable
# EXEC = dungeon_cpp

# # Default target
# all: $(EXEC)

# # Link final binary
# $(EXEC): $(OBJ)
# 	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# # Make sure build directory exists
# $(OBJ): | build

# build:
# 	mkdir -p build

# # Compile each source file into object file in build/
# build/%.o: src/%.cpp
# 	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# # Clean build artifacts
# clean:
# 	rm -rf build $(EXEC)

# # Run the game
# run: $(EXEC)
# 	./$(EXEC) $(ARGS)

# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g
LDFLAGS = -lncurses

# Include paths
INCLUDES = -Iinclude

# Source files in src/
SRC = \
	src/main.cpp \
	src/Dungeon.cpp \
	src/Monster.cpp \
	src/Player.cpp \
	src/Room.cpp \
	src/Event.cpp \
	src/SaveLoad.cpp \
	src/MonsterParser.cpp \
	src/MonsterDescription.cpp \
	src/ObjectDescription.cpp \
	src/Object.cpp \
	src/ObjectParser.cpp

# Object files in build/
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))

# Output executable
EXEC = dungeon_cpp

# Default target
all: $(EXEC)

# Link final binary
$(EXEC): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Ensure build directory exists
$(OBJ): | build

build:
	mkdir -p build

# Compile each source file into object file in build/
build/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf build $(EXEC)

# Run the game with optional ARGS (e.g. make run ARGS="--parse-monsters")
run: $(EXEC)
	./$(EXEC) $(ARGS)
