BUILD_FOLDER := cmake-build-debug
DEBUG_BUILD_FILE := build-debug
PROJECT_NAME := {{PROJECT_NAME}}
CLEAR_CONSOLE := 1

.PHONY: help check-platform build build-clean build-debug run

default: help
help:
	@echo "Available commands:"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-20s\033[0m %s\n", $$1, $$2}'

check-platform:
	@if ! ([ "$$OS" = "Windows_NT" ] || [ "$$(uname)" = "Darwin" ] || [ "$$(uname)" = "Linux" ]); then \
		 echo "Unsupported platform. The engine only supports MacOS, Linux and Windows"; \
		 exit 1; \
	fi

build: check-platform ## Build's the game executable
	@if [ $(CLEAR_CONSOLE) -eq 1 ]; then \
		clear; \
	fi; \
	cmake -B $(BUILD_FOLDER) -S .; \
	cmake --build $(BUILD_FOLDER); \

build-clean: check-platform ## Build's a cleaned version of the game executable
	@clear; \
	cmake --build $(BUILD_FOLDER) --target clean; \
	$(MAKE) build CLEAR_CONSOLE=0

build-debug: check-platform ## Builds the game executable in debug mode to pinpoint crash points
	@clear;\
	mkdir -p $(DEBUG_BUILD_FILE);\
	cmake -B $(DEBUG_BUILD_FILE) -S . -DCMAKE_BUILD_TYPE=Debug;\
	cmake --build $(DEBUG_BUILD_FILE);\
	if [ "$$OS" = "Windows_NT" ]; then \
		if where windbg >nul 2>&1; then \
			windbg $(DEBUG_BUILD_FILE)/$(PROJECT_NAME); \
		else \
			echo "Please install WinDbg to use the build-debug command."; \
			echo -e "To install WinDbg run: \033[1mwinget install --id Microsoft.WinDbg\033[0m"; \
		fi; \
	elif [ "$$(uname)" = "Darwin" ]; then \
		lldb $(DEBUG_BUILD_FILE)/$(PROJECT_NAME);\
	elif [ "$$(uname)" = "Linux" ]; then \
		gdb $(DEBUG_BUILD_FILE)/$(PROJECT_NAME);\
	fi; \
	rm -rf $(DEBUG_BUILD_FILE)

run: ## Launches the game
	@$(BUILD_FOLDER)/$(PROJECT_NAME)