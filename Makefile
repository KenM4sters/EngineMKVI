CC=clang++

BUILD_DIR:= ./bin/app

COMPILER_FLAGS:= -std=c++17
INCLUDE_FLAGS:= -I./src -I$(VULKAN_SDK)/include -I$(VULKAN_SDK)/lib -I./include -framework Cocoa -framework IOkit
LINKER_FLAGS:= -lvulkan -L./lib -lglfw3
SRC_FLAGS:= ./src/*.cpp

.PHONY: build clean

build:
	$(CC) $(COMPILER_FLAGS) $(SRC_FLAGS) -o $(BUILD_DIR) $(INCLUDE_FLAGS) $(LINKER_FLAGS)

clean:
	rm -rf $(BUILD_DIR)

