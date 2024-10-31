cmake_minimum_required(VERSION 3.29)
project(GameEngine)

set(CMAKE_CXX_STANDARD 20)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

# Find all .cpp and .h files in the Engine directory and its subdirectories
file(GLOB_RECURSE ENGINE_SOURCES "Engine/*.cpp" "include/*.h")

# Create the GameEngineLib as a static library
add_library(GameEngineLib STATIC ${ENGINE_SOURCES})

# Make sure a version was indicated
if (DEFINED VERSION AND NOT "${VERSION}" STREQUAL "")
    # Check if VERSION matches the pattern [0-9]*.[0-9]*.[0-9]*
    string(REGEX MATCH "^[0-9]+\\.[0-9]+\\.[0-9]+$" VERSION_FORMAT "${VERSION}")

    if (VERSION_FORMAT)
        set_target_properties(GameEngineLib PROPERTIES
            OUTPUT_NAME "GameEngineLib-${VERSION}"
            PREFIX ""
        )
    else()
        message(FATAL_ERROR "Invalid VERSION format. Expected format: <major>.<minor>.<patch> (e.g., 1.0.0)")
    endif()
else ()
    message(FATAL_ERROR "Missing library version")
endif ()

# Define paths to the static versions of GLEW and GLFW libraries
set(GLEW_STATIC_LIB /opt/homebrew/opt/glew/lib/libGLEW.a)
set(GLFW_STATIC_LIB /opt/homebrew/opt/glfw/lib/libglfw3.a)

# Link OpenGL, GLEW, and GLFW static libraries to GameEngineLib
target_link_libraries(GameEngineLib PUBLIC
        OpenGL::GL
        ${GLEW_STATIC_LIB}
        ${GLFW_STATIC_LIB}
        "-framework CoreFoundation"
        "-framework IOKit"
        "-framework Cocoa"
)