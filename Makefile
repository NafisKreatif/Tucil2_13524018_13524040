# Detect the os: windows or linux, other os is not really supported
DETECTED_OS := 
ifdef OS
	DETECTED_OS = Windows
else
	DETECTED_OS = $(shell uname)
endif

# Target and directory
MAIN_TARGET		:= bin/main
RENDERER_TARGET	:= bin/renderer
SRC_DIR     	:= src
BUILD_DIR   	:= build

# Files
SRC_FILES   := 
ifeq ($(DETECTED_OS), Windows)
	SRC_FILES = $(filter-out $(SRC_DIR)/renderer.cpp,$(foreach dir,$(SRC_DIR),$(wildcard $(dir)/*.cpp) $(wildcard $(dir)/*/*.cpp) $(wildcard $(dir)/*/*/*.cpp)))
else
	SRC_FILES = $(filter-out $(SRC_DIR)/renderer.cpp,$(shell find $(SRC_DIR) -name "*.cpp"))
endif
OBJ_FILES   := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRC_FILES))

# Compiler and Flags
CC          := g++
CFLAGS      := -Wall -Wextra -I$(SRC_DIR)
DEBUG_FLAGS := -g
SFML_FLAGS  := -lsfml-graphics -lsfml-window -lsfml-system

# Define phony MAIN_TARGETs
.PHONY: all debug renderer run clean clean-bin clean-all

# Build the program
all: $(MAIN_TARGET)

# link the final executable
$(MAIN_TARGET): $(OBJ_FILES)
ifeq ($(DETECTED_OS), Windows)
	@if not exist "$(dir $@)" md "$(dir $@)"
else
	@mkdir -p $(dir $@)
endif
	$(CC) $(OBJ_FILES) -o $@

# compile each .cpp into .o under build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
ifeq ($(DETECTED_OS), Windows)
	@if not exist "$(dir $@)" md "$(dir $@)"
else
	@mkdir -p $(dir $@)
endif
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -c $< -o $@

# Debug build
debug: CFLAGS += -DDEBUG
debug: all

# Run the program
run: all
	@./$(MAIN_TARGET)

# Clean build files
clean:
	@echo "Cleaning build files..."
ifeq ($(DETECTED_OS), Windows)
	@if exist "$(BUILD_DIR)" @rmdir /Q /S "$(BUILD_DIR)"
	@if exist "$(subst /,\,$(MAIN_TARGET)).exe" @del "$(subst /,\,$(MAIN_TARGET)).exe"
	@if exist "$(subst /,\,$(RENDERER_TARGET)).exe" @del "$(subst /,\,$(RENDERER_TARGET)).exe"
else
	rm -rf $(BUILD_DIR) $(MAIN_TARGET) $(RENDERER_TARGET)
endif

# Clean binary file
clean-bin:
	@echo "Removing bin directory..."
ifeq ($(DETECTED_OS), Windows)
	@rmdir /Q /S "bin\"
else
	rm -rf bin
endif

clean-all: clean clean-bin

print:
	@echo $(DETECTED_OS)
	@echo $(SRC_FILES)
	@echo $(OBJ_FILES)

renderer:
	@echo "Building renderer..."
	$(CC) -o $(RENDERER_TARGET) src/renderer.cpp src/stima/geometry/Point3D.cpp -I$(SRC_DIR) $(SFML_FLAGS)