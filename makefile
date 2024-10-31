buildlib:
	@clear
	@mkdir -p build
	@rm -rf build/*
	@cmake -B build -S . -DVERSION=$(V) || rm -rf build/*
	@cmake --build build
	@rm -f GameEngineLib-[0-9]*.[0-9]*.[0-9]*.a
	@mv -f ./build/GameEngineLib-[0-9]*.[0-9]*.[0-9]*.a ./

THIRD_PARTY_LOCATION := include/third_party

movelib:
	@if ! brew list glew >/dev/null 2>&1; then \
		echo "GLEW not found. Installing GLEW..."; \
		brew install glew; \
	fi
	@if ! brew list glfw >/dev/null 2>&1; then \
		echo "GLFW not found. Installing GLFW..."; \
		brew install glfw; \
	fi
	@rm -rf $(THIRD_PARTY_LOCATION)/*
	@mkdir -p $(THIRD_PARTY_LOCATION)/glew
	@mkdir -p $(THIRD_PARTY_LOCATION)/glwf
	@cp -rp /opt/homebrew/opt/glew/include/GL/* $(THIRD_PARTY_LOCATION)/glew
	@cp -rp /opt/homebrew/opt/glfw/include/GLFW/* $(THIRD_PARTY_LOCATION)/glwf
