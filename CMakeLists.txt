cmake_minimum_required(VERSION 3.29)
project(GameEngine)

set(CMAKE_CXX_STANDARD 20)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)
pkg_check_modules(GLM REQUIRED glm)

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
)

set(VERSION 0.1.0)
set_target_properties(GameEngineLib PROPERTIES
    OUTPUT_NAME "GameEngineLib-${VERSION}"
    PREFIX ""
)

# Define paths to the static versions of GLEW and GLFW libraries
set(GLEW_STATIC_LIB /opt/homebrew/opt/glew/lib/libGLEW.a)
set(GLFW_STATIC_LIB /opt/homebrew/opt/glfw/lib/libglfw3.a)
set(GLM_STATIC_LIB /opt/homebrew/opt/glm/lib/libglm.dylib)

# Link OpenGL, GLEW, and GLFW static libraries to GameEngineLib
target_link_libraries(GameEngineLib PUBLIC
    OpenGL::GL
    ${GLEW_STATIC_LIB}
    ${GLFW_STATIC_LIB}
    ${GLM_STATIC_LIB}
    "-framework CoreFoundation"
    "-framework IOKit"
    "-framework Cocoa"
)

target_include_directories(GameEngineLib PUBLIC
    EngineFiles
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
)