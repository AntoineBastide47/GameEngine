BUILD_FOLDER := cmake-build-debug

build_lib:
	@clear
	@cmake -B $(BUILD_FOLDER) -S .
	@cmake --build $(BUILD_FOLDER)
	@rm -f ./EngineFiles/*.a
	@mv -f ./$(BUILD_FOLDER)/*.a EngineFiles

build_lib_clean:
	@clear
	@rm -rf $(BUILD_FOLDER)
	@cmake -B $(BUILD_FOLDER) -S .
	@cmake --build $(BUILD_FOLDER)
	@rm -f ./EngineFiles/*.a
	@mv -f ./$(BUILD_FOLDER)/*.a EngineFiles