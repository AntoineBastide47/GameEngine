build_lib:
	@clear
	@rm -rf build
	@mkdir -p build
	@cmake -B build -S .
	@cmake --build build
	@rm -f ./EngineFiles/GameEngineLib-[0-9]*.[0-9]*.[0-9]*.a
	@mv -f ./build/GameEngineLib-[0-9]*.[0-9]*.[0-9]*.a ./EngineFiles