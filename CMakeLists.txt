cmake_minimum_required(VERSION 3.29)
project(GameEngine)

set(CMAKE_CXX_STANDARD 20)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)
pkg_check_modules(GLM REQUIRED glm)

# Create the Engine2D as a static library
add_library(Engine2D STATIC
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
    EngineFiles/Common/Event.h
    Engine/Input/Keyboard.cpp
    EngineFiles/Input/Keyboard.h
    Engine/Input/Mouse.cpp
    EngineFiles/Input/Mouse.h
    Engine/Input/Gamepad.cpp
    EngineFiles/Input/Gamepad.h
    EngineFiles/Input/InputContexts.h
)

# Some optimizations
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
set_target_properties(Engine2D PROPERTIES POSITION_INDEPENDENT_CODE ON)

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

set(VERSION 0.2.0)
set_target_properties(Engine2D PROPERTIES
    OUTPUT_NAME Engine2D-${VERSION}
    PREFIX ""
)

target_link_libraries(Engine2D PUBLIC
    ${GLEW_LIBRARIES}
    ${GLFW3_LIBRARIES}
    ${OpenGL_LIBRARIES}
    glm
    assets
    cpptrace::cpptrace
)

if (APPLE)
  target_link_libraries(Engine2D PUBLIC
      "-framework CoreFoundation"
      "-framework IOKit"
      "-framework Cocoa"
  )
elseif (WIN32)
  message(STATUS "Window build platform not supported yet.")
elseif (UNIX)
  find_package(Threads REQUIRED)
  target_link_libraries(Engine2D PUBLIC
    Threads::Threads
  )
else()
  message(FATAL_ERROR "Unknown build platform.")
endif ()

target_include_directories(Engine2D PUBLIC
    EngineFiles
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${cpptrace_SOURCE_DIR}/include
)