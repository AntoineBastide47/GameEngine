cmake_minimum_required(VERSION 3.29)
project(GameEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")

# Find required packages
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)
pkg_check_modules(GLM REQUIRED glm)

# Include CMakeRC
include(CMakeRC.cmake)
cmrc_add_resource_library(assets NAMESPACE resources ${CMAKE_SOURCE_DIR}/Assets)

# Include cpptrace to be able to print stack traces
include(FetchContent)
FetchContent_Declare(
    cpptrace
    GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
    GIT_TAG        9269a72c54e0faab981aa8710b65858e2eb1f59c # <HASH or TAG>
)
FetchContent_MakeAvailable(cpptrace)

# Create the GameEngineLib as a static library
add_library(GameEngineLib STATIC
    EngineFiles/Common/Macros.h
    EngineFiles/2D/Rendering/stb_image.h
    EngineFiles/Common/RenderingHeaders.h
    Engine/2D/Game2D.cpp
    EngineFiles/2D/Game2D.h
    Engine/2D/Rendering/Shader.cpp
    EngineFiles/2D/Rendering/Shader.h
    Engine/2D/Rendering/Texture2D.cpp
    EngineFiles/2D/Rendering/Texture2D.h
    Engine/2D/ResourceManager.cpp
    EngineFiles/2D/ResourceManager.h
    Engine/2D/Rendering/SpriteRenderer.cpp
    EngineFiles/2D/Rendering/SpriteRenderer.h
    Engine/2D/Types/Vector2.cpp
    EngineFiles/2D/Types/Vector2.h
    Engine/2D/Components/Transform2D.cpp
    EngineFiles/2D/Components/Transform2D.h
    Engine/2D/Entity2D.cpp
    EngineFiles/2D/Entity2D.h
    Engine/Common/Log.cpp
    EngineFiles/Common/Log.h
    EngineFiles/Common/PrimitiveTypes.h
    EngineFiles/Common/Event.h
    Engine/Input/Keyboard.cpp
    EngineFiles/Input/Keyboard.h
)

set(VERSION 0.1.3)
set_target_properties(GameEngineLib PROPERTIES
    OUTPUT_NAME "GameEngineLib-${VERSION}"
    PREFIX ""
)

# Define paths to the static versions of GLEW and GLFW libraries
set(GLEW_STATIC_LIB /opt/homebrew/opt/glew/lib/libGLEW.a)
set(GLFW_STATIC_LIB /opt/homebrew/opt/glfw/lib/libglfw3.a)
set(GLM_STATIC_LIB /opt/homebrew/opt/glm/lib/libglm.dylib)

target_link_libraries(GameEngineLib PUBLIC
    OpenGL::GL
    ${GLEW_STATIC_LIB}
    ${GLFW_STATIC_LIB}
    ${GLM_STATIC_LIB}
    "-framework CoreFoundation"
    "-framework IOKit"
    "-framework Cocoa"
    assets
    cpptrace::cpptrace
)

target_include_directories(GameEngineLib PUBLIC
    EngineFiles
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${cpptrace_SOURCE_DIR}/include
)