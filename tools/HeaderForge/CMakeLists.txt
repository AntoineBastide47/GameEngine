cmake_minimum_required(VERSION 3.31)
project(HeaderForge VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Release")

# Define the executable
add_executable(${PROJECT_NAME}
    main.cpp
    src/Parser.cpp
    include/Parser.hpp
)
target_include_directories(${PROJECT_NAME} PUBLIC include)

# === LLVM & Clang ===
find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}
    clangTooling clangFrontend clangASTMatchers clangAST clangBasic
)

# --------------------------
# Build Configs
# --------------------------

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -O3 -DNDEBUG -finline-functions -fomit-frame-pointer
        -ffunction-sections -fdata-sections
    )
    target_link_options(${PROJECT_NAME} PRIVATE -Wl,-O2)
    set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${PROJECT_NAME}>)
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        set(SANITIZER_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
        set(EXTRA_DEBUG_FLAGS "-g -O1")
        set(ASAN_OPTIONS_FLAGS "-fsanitize-recover=address -fsanitize=undefined -fsanitize-address-use-after-scope")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS} ${EXTRA_DEBUG_FLAGS} ${ASAN_OPTIONS_FLAGS}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SANITIZER_FLAGS} ${EXTRA_DEBUG_FLAGS} ${ASAN_OPTIONS_FLAGS}")
        set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} ${SANITIZER_FLAGS} ${EXTRA_DEBUG_FLAGS} ${ASAN_OPTIONS_FLAGS}")
        add_compile_definitions(ASAN_OPTIONS="verbosity=1:halt_on_error=1:detect_leaks=1:symbolize=1")
    endif ()
endif ()

string(REGEX REPLACE "([A-Z])" "-\\1" PROJECT_NAME_DASHED "${PROJECT_NAME}")
string(TOLOWER "${PROJECT_NAME_DASHED}" PROJECT_NAME_DASHED)
string(REGEX REPLACE "^-+" "" PROJECT_NAME_DASHED "${PROJECT_NAME_DASHED}")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:${PROJECT_NAME}>
        ${CMAKE_CURRENT_BINARY_DIR}/../../../${PROJECT_NAME_DASHED}$<TARGET_FILE_SUFFIX:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E remove
        $<TARGET_FILE:${PROJECT_NAME}>
)