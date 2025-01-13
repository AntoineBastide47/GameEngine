.PHONY: help create-project

default: help
help:
	@echo "Available commands:"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-20s\033[0m %s\n", $$1, $$2}'

create-project: ## Starts the CLI to create a new project
	@clear; \
	if ! ([ "$$OS" = "Windows_NT" ] || [ "$$(uname)" = "Darwin" ] || [ "$$(uname)" = "Linux" ]); then \
		 echo "Unsupported platform. The engine only supports MacOS, Linux and Windows"; \
		 exit 1; \
	fi; \
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
		fi; \
	done; \
	cd "$$location/$$name"; \
	if [ "$$OS" = "Windows_NT" ]; then \
    		powershell -Command "Get-ChildItem -Recurse -File | ForEach-Object { (Get-Content \$$_.FullName) -replace '{{PROJECT_NAME}}', '$(name)' | Set-Content \$$_.FullName }"; \
	elif [ "$$(uname)" = "Darwin" ]; then find . -type f -exec sed -i '' "s/{{PROJECT_NAME}}/$$name/g" {} +; \
	elif [ "$$(uname)" = "Linux" ]; then find . -type f -exec sed -i "s/{{PROJECT_NAME}}/$$name/g" {} +; \
	fi; \
	mv "./GameCode/{{PROJECT_NAME}}.cpp" "./GameCode/$$name.cpp"; \
	mv "./GameCode/{{PROJECT_NAME}}.h" "./GameCode/$$name.h"; \
	if [ "$$OS" = "Windows_NT" ]; then \
		if (NET SESSION 1>NUL 2>NUL); then cmd /c "mklink /D $(realpath Include) $$location/$$name/EngineInclude"; \
		else \
			echo "Skipping symbolic link creation. Copying directory instead."; \
			cp -r $(realpath Include) "$$location/$$name/EngineInclude"; \
		fi; \
	else ln -s $(realpath Include) "$$location/$$name/EngineInclude"; fi; \
	echo "Project '$$name' of type '$$type' created at '$$location/$$name'."