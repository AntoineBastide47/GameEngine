BUILD_FOLDER := cmake-build-debug
BUILD_TYPE := ""

.PHONY: help check-platform install-dependencies build build-debug build-release build-tool build-tools run-stripper create-project

default: help
help: check-platform
	@echo "Available commands:"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-20s\033[0m %s\n", $$1, $$2}'

check-platform:
	@if ! ([ "$$OS" = "Windows_NT" ] || [ "$$(uname)" = "Darwin" ] || [ "$$(uname)" = "Linux" ]); then \
		 echo "Unsupported platform. The engine only supports MacOS, Linux and Windows"; \
		 exit 1; \
	fi

install-dependencies: check-platform ## Installs all the dependencies required to build the engine
	@if [ "$$(uname)" = "Darwin" ]; then \
  		brew install glew glfw glm; \
  	elif [ "$$(uname)" = "Linux" ]; then \
  		sudo apt-get update && sudo apt-get install -y libglew-dev libglfw3-dev libglm-dev; \
  	else \
  	  echo "Dependencies installation is not configured for windows. Please install GLEW, GLFW, and GLM manually."; \
  	fi

build-debug: check-platform ## Build's the engine static library in debug mode
	@make build BUILD_TYPE="Debug"

build-release: check-platform ## Build's the engine static library in release mode
	@make build BUILD_TYPE="Release"

build:
	@clear; \
	if [ $(BUILD_TYPE) = "" ]; then \
	  echo "Please avoid using \033[1mmake build\033[0m to build the engine.\nInstead, you should use \033[1mmake build-debug\033[0m or \033[1mmake build-release\033[0m"; \
	  exit 1; \
  	fi; \
	rm -rf Engine/[a-zA-Z0-9]*-[0-9]*.[0-9]*.[0-9]*.dylib; \
	rm -rf Engine.zip; \
  	cmake -B $(BUILD_FOLDER) -S . -DCMAKE_BUILD_TYPE=$(BUILD_TYPE); \
  	if [ $(BUILD_TYPE) = "Release" ]; then cmake --build $(BUILD_FOLDER) --target clean; fi; \
	cmake --build $(BUILD_FOLDER); \
	mv -f $(BUILD_FOLDER)/[a-zA-Z0-9]*-[0-9]*.[0-9]*.[0-9]*.dylib Engine/Include; \
	if [ $(BUILD_TYPE) = "Release" ]; then zip -r Engine.zip Engine; fi; \