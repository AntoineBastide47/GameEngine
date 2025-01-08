BUILD_FOLDER := cmake-build-debug
CLEAR_CONSOLE := 1

default: help

help:
	@echo "Available commands:"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-20s\033[0m %s\n", $$1, $$2}'

install-dependencies: ## Installs all the dependencies required to build the engine and games
	@if [ "$$(uname)" = "Darwin" ]; then \
  		brew install glew glfw glm; \
  	elif [ "$$(uname)" = "Linux" ]; then \
  		sudo apt-get update && sudo apt-get install -y libglew-dev libglfw3-dev libglm-dev; \
  	else \
  	  echo "Dependencies installation is not configured for this system. Please install GLEW, GLFW, and GLM manually."; \
  	fi

build: ## Build's the engine static library
	@if [ $(CLEAR_CONSOLE) -eq 1 ]; then \
		clear; \
	fi; \
	cmake -B $(BUILD_FOLDER) -S .; \
	cmake --build $(BUILD_FOLDER); \
	rm -f ./EngineFiles/*.a; \
	mv -f ./$(BUILD_FOLDER)/[a-zA-Z0-9]*-[0-9]*.[0-9]*.[0-9]*.a EngineFiles;

build-clean: ## Build's a cleaned version of the engine static library
	@clear; \
	cmake --build $(BUILD_FOLDER) --target clean; \
	$(MAKE) build CLEAR_CONSOLE=0

create-project: ## Starts the CLI to create a new project
	@clear
	@if ! ([ "$$(uname)" = "Darwin" ] || [ "$$(uname)" = "Linux" ] || [ "$$OS" = "Windows_NT" ]); then \
         echo "Unsupported platform. The engine only supports MacOS, Linux and Windows"; \
         exit 1; \
    fi
	@echo "GameEngine Project creation CLI:"
	@validType=0; \
	while [ $$validType -eq 0 ]; do \
		read -p "Project Type (2D): " type; \
		if [ -d ./Templates/$$type ]; then \
			type=$$(echo "$$type" | sed 's/^ *//; s/ *$$//'); \
			validType=1; \
		else \
			echo "ERROR: Template for '$$type' does not exist."; \
			echo "Currently supported templates: 2D"; \
		fi; \
	done; \
	validLocation=0; \
	while [ $$validLocation -eq 0 ]; do \
		read -p "Project Location: " location; \
		location=$$(echo "$$location" | sed 's/^ *//; s/ *$$//'); \
		location=$$(echo "$$location" | tr '\\\\' '/'); \
		if [ -z "$$location" ]; then \
			echo "ERROR: Location cannot be empty"; \
		elif [ -d "$$location" ]; then \
			validLocation=1; \
		else \
			echo "ERROR: Either '$$location' does not exist or it is not a folder"; \
		fi; \
	done; \
	validName=0; \
	while [ $$validName -eq 0 ]; do \
		read -p "Project Name: " name; \
		name=$$(echo "$$name" | tr -d ' '); \
		if [ -z "$$name" ]; then \
			echo "ERROR: Name cannot be empty"; \
		elif [ -d "$$location/$$name" ]; then \
			echo "ERROR: A directory with the name '$$name' already exists at the project location"; \
			echo "$$location/$$name"; \
		else \
			validName=1; \
			mkdir -p "$$location/$$name"; \
			cp -rp ./Templates/$$type/* "$$location/$$name"; \
		fi; \
	done; \
	cd "$$location/$$name"; \
	if [ "$$(uname)" = "Darwin" ]; then \
    		find . -type f -exec sed -i '' "s/{{PROJECT_NAME}}/$$name/g" {} +; \
	elif [ "$$(uname)" = "Linux" ]; then \
		find . -type f -exec sed -i "s/{{PROJECT_NAME}}/$$name/g" {} +; \
	elif [ "$$OS" = "Windows_NT" ]; then \
		powershell -Command "Get-ChildItem -Recurse -File | ForEach-Object { (Get-Content \$$_.FullName) -replace '{{PROJECT_NAME}}', '$(name)' | Set-Content \$$_.FullName }"; \
	else \
		echo "Unsupported platform"; \
	fi; \
	mv "./GameCode/{{PROJECT_NAME}}.cpp" "./GameCode/$$name.cpp"; \
	mv "./GameCode/{{PROJECT_NAME}}.h" "./GameCode/$$name.h"; \
	ln -s $(realpath EngineFiles) "$$location/$$name/EngineFiles"; \
	echo "Project '$$name' of type '$$type' created at '$$location/$$name'."