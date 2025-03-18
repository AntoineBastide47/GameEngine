BUILD_FOLDER := cmake-build-debug
PROJECT_NAME := {{PROJECT_NAME}}
BUILD_TYPE := ""

.PHONY: help check-platform build-release build-debug build run

default: help
help:
	@echo "Available commands:"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-20s\033[0m %s\n", $$1, $$2}'

check-platform:
	@if ! ([ "$$OS" = "Windows_NT" ] || [ "$$(uname)" = "Darwin" ] || [ "$$(uname)" = "Linux" ]); then \
		 echo "Unsupported platform. The engine only supports MacOS, Linux and Windows"; \
		 exit 1; \
	fi

build-debug: check-platform ## Build's the project in debug mode
	@make build BUILD_TYPE="Debug"

build-release: check-platform ## Build's the project  in release mode
	@make build BUILD_TYPE="Release"

build:
	@clear; \
	if [ $(BUILD_TYPE) = "" ]; then \
		echo "Please avoid using \033[1mmake build\033[0m to build the engine.\nInstead, you should use \033[1mmake build-debug\033[0m or \033[1mmake build-release\033[0m"; \
		exit 1; \
	fi; \
	cmake -B $(BUILD_FOLDER) -S . -DCMAKE_BUILD_TYPE=$(BUILD_TYPE); \
	if [ $(BUILD_TYPE) = "Release" ]; then \
		cmake --build $(BUILD_FOLDER) --target clean; \
	fi; \
	cmake --build $(BUILD_FOLDER); \
	mv $(BUILD_FOLDER)/$(PROJECT_NAME) .

run: ## Launches the game
	@./$(PROJECT_NAME)