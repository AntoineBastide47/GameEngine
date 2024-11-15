build_lib:
	@clear
	@cmake -B cmake-build-debug -S .
	@cmake --build cmake-build-debug
	@rm -f ./EngineFiles/*.a
	@mv -f ./cmake-build-debug/*.a EngineFiles

build_lib_clean:
	@clear
	@rm -rf cmake-build-debug
	@cmake -B cmake-build-debug -S .
	@cmake --build cmake-build-debug
	@rm -f ./EngineFiles/*.a
	@mv -f ./cmake-build-debug/*.a EngineFiles