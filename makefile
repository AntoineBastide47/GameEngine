BUILD_FOLDER = cmake-build-debug
BUILD_TYPE =
VENDOR_LOCATION = ./vendor

.PHONY: help check-platform build-dependencies build build-debug build-release create-project dependencies

default: help

check-platform:
	@if ! ([ "$$OS" = "Windows_NT" ] || [ "$$(uname)" = "Darwin" ] || [ "$$(uname)" = "Linux" ]); then \
		 echo "Unsupported platform. The engine only supports MacOS, Linux and Windows for now."; \
		 exit 1; \
	fi

help: check-platform
	@echo "Available commands:"; \
	grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-20s\033[0m %s\n", $$1, $$2}'

dependencies: check-platform ## Update's the github submodules linked to this project
	@if [ ! -d $(VENDOR_LOCATION)/glfw ]; then \
		git submodule add https://github.com/glfw/glfw.git $(VENDOR_LOCATION)/glfw; \
	fi; \
	if [ ! -d $(VENDOR_LOCATION)/glm ]; then \
		git submodule add https://github.com/g-truc/glm.git $(VENDOR_LOCATION)/glm; \
	fi; \
	if [ ! -d $(VENDOR_LOCATION)/cpptrace ]; then \
		git submodule add https://github.com/jeremy-rifkin/cpptrace.git $(VENDOR_LOCATION)/cpptrace; \
	fi; \
	git submodule update --init --recursive

build-debug: ## Build's the engine static library in debug mode
	@make build BUILD_TYPE="Debug"

build-release: ## Build's the engine static library in release mode
	@make build BUILD_TYPE="Release"

build: check-platform
	@clear; \
	if [ $(BUILD_TYPE) = "" ]; then \
		echo "Please avoid using \033[1mmake build\033[0m to build the engine.\nInstead, you should use \033[1mmake build-debug\033[0m or \033[1mmake build-release\033[0m"; \
		exit 1; \
  	fi; \
  	make dependencies; \
	rm -rf include/*-[0-9]*.[0-9]*.[0-9]*.*; \
  	cmake -B $(BUILD_FOLDER) -S . -DCMAKE_BUILD_TYPE=$(BUILD_TYPE); \
  	if [ $(BUILD_TYPE) = "Release" ]; then \
  	  	cmake --build $(BUILD_FOLDER) --target clean; \
  	fi; \
	cmake --build $(BUILD_FOLDER); \
	mv -f $(BUILD_FOLDER)/*-[0-9]*.[0-9]*.[0-9]*.* include

create-project: check-platform ## Starts the CLI to create a new project
	@clear; \
	echo "GameEngine Project creation CLI:"; \
	validType=0; \
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
		if [ -z "$$location" ]; then echo "ERROR: Location cannot be empty"; \
		elif [ -d "$$location" ]; then validLocation=1; \
		else echo "ERROR: Either '$$location' does not exist or it is not a folder"; \
		fi; \
	done; \
	validName=0; \
	while [ $$validName -eq 0 ]; do \
		read -p "Project Name: " name; \
		name=$$(echo "$$name" | tr -d ' '); \
		if [ -z "$$name" ]; then echo "ERROR: Name cannot be empty"; \
		elif [ -d "$$location/$$name" ]; then \
			echo "ERROR: A directory with the name '$$name' already exists at the project location"; \
			echo "$$location/$$name"; \
		else \
			validName=1; \
			mkdir -p "$$location/$$name"; \
			cp -rp ./Templates/$$type/* "$$location/$$name"; \
			cp -rp ./include/Shaders/* "$$location/$$name/Engine/Shaders"; \
		fi; \
	done; \
	FROM="$$(realpath $$location/$$name)"; \
	TO="$$(realpath .)"; \
	cd "$$location/$$name"; \
	if [ "$$OS" = "Windows_NT" ]; then \
		powershell -Command "Get-ChildItem -Recurse -File | ForEach-Object { (Get-Content \$$_.FullName) -replace '{{PROJECT_NAME}}', '$(name)' | Set-Content \$$_.FullName }"; \
		powershell -Command "Get-ChildItem -Recurse -File | ForEach-Object { (Get-Content $_.FullName) -replace '{{ENGINE_LOCATION}}', '$(REL_PATH)' | Set-Content $_.FullName }"; \
	else \
		REL="$$(awk -v from="$$FROM" -v to="$$TO" ' \
        	BEGIN { \
        		n=split(from, f, "/"); m=split(to, t, "/"); \
        		while (f[i+1] == t[i+1]) i++; \
        		for (j=i+1; j<=n; j++) up=up"../"; \
        		for (j=i+1; j<=m; j++) down=down t[j]"/"; \
        		print up down \
        	}')"; \
		if [ "$$(uname)" = "Darwin" ]; then \
			find . -type f -exec sed -i '' "s/{{PROJECT_NAME}}/$$name/g" {} +; \
	  		find . -type f -exec sed -i '' "s|{{ENGINE_LOCATION}}|$$REL|g" {} +; \
		elif [ "$$(uname)" = "Linux" ]; then \
			find . -type f -exec sed -i "s/{{PROJECT_NAME}}/$$name/g" {} +; \
			find . -type f -exec sed -i "s|{{ENGINE_LOCATION}}|$$REL|g" {} +; \
		fi; \
	fi; \
	mv "./Game/src/{{PROJECT_NAME}}.cpp" "./Game/src/$$name.cpp"; \
	mv "./Game/include/{{PROJECT_NAME}}.hpp" "./Game/include/$$name.hpp"; \
	echo "Project '$$name' of type '$$type' created at '$$location/$$name'."


#if [ "$$OS" = "Windows_NT" ]; then \
	if (NET SESSION 1>NUL 2>NUL); then \
	  cmd /c "mklink /D $(realpath include) $$location/$$name/EngineInclude"; \
	  cmd /c "mklink /D $(realpath vendor) $$location/$$name/vendor"; \
	else \
		cp -r $(realpath EngineInclude) "$$location/$$name/EngineInclude"; \
		cp -r $(realpath vendor) "$$location/$$name/vendor"; \
	fi; \
else \
  ln -s $(realpath EngineInclude) "$$location/$$name/EngineInclude"; \
  ln -s $(realpath vendor) "$$location/$$name/vendor"; \
fi