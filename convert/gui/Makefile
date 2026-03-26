BUILD_DIR := build
CMAKE ?= cmake

.PHONY: all configure build run clean

all: build

configure:
	$(CMAKE) -S . -B $(BUILD_DIR)

build: configure
	$(CMAKE) --build $(BUILD_DIR)

run: build
	./$(BUILD_DIR)/ImageConvertQt

clean:
	rm -rf $(BUILD_DIR)
