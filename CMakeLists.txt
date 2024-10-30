cmake_minimum_required(VERSION 3.29)
project(GameEngine)

set(CMAKE_CXX_STANDARD 20)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

# Create the shared library
add_library(GameEngineLib SHARED
        Engine/Common/Rendering/OpenGL.cpp
        Engine/Common/Rendering/OpenGL.h
        Engine/2D/Game2D.cpp
        Engine/2D/Game2D.h
        Engine/Common/Headers/RenderingHeaders.h
        Engine/Common/Headers/CommonHeaders.h
        Engine/Common/Headers/PrimitiveTypes.h
)

set_target_properties(GameEngineLib PROPERTIES
        VERSION 0.0.1               # Library version
        SOVERSION 0                 # ABI version (major version)
        OUTPUT_NAME "GameEngineLib" # Set output name without "lib" prefix
        PREFIX ""                   # Remove default "lib" prefix
)

# Link OpenGL, GLEW, and GLFW to the GameEngine library
target_link_libraries(GameEngineLib PRIVATE
        OpenGL::GL
        GLEW::GLEW
        glfw
)

# If you have additional include directories, you can add them like this:
target_include_directories(GameEngineLib PRIVATE
        ${GLEW_INCLUDE_DIRS}
        ${GLFW_INCLUDE_DIRS}
)