cmake_minimum_required(VERSION 3.29)
project(GameEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Debug")

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)

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
    Engine/2D/Components/Component2D.cpp
    EngineFiles/2D/Components/Component2D.h
    Engine/2D/Physics/Collisions.cpp
    EngineFiles/2D/Physics/Collisions.h
    Engine/2D/Physics/Physics2D.cpp
    EngineFiles/2D/Physics/Physics2D.h
    Engine/2D/Physics/Rigidbody2D.cpp
    EngineFiles/2D/Physics/Rigidbody2D.h
    Engine/2D/Rendering/ShapeRenderer.cpp
    EngineFiles/2D/Rendering/ShapeRenderer.h
    EngineFiles/Common/Property.h
    Engine/2D/Physics/CollisionManifold.cpp
    EngineFiles/2D/Physics/CollisionManifold.h
    Engine/Types/float01.cpp
    EngineFiles/Types/float01.h
)

# Add RTTI
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-frtti)
elseif(MSVC)
  add_compile_options(/GR)
endif()


if(CMAKE_BUILD_TYPE STREQUAL "Release")
  target_compile_options(Engine2D PRIVATE
      -O3                     # General optimization
      -DNDEBUG                # Disable assertions
      -finline-functions      # Enable inlining
      -flto                   # Link-Time Optimization
      -fomit-frame-pointer    # Free up a register
      -ffunction-sections     # Enable section-based optimization
      -fdata-sections         # Enable data section optimization
  )
  target_link_options(Engine2D PRIVATE
      -Wl,-O2                 # Optimize linker
  )
  set_target_properties(Engine2D PROPERTIES POSITION_INDEPENDENT_CODE ON)

  # Add stripping for Release builds
  # TODO: make this optional ? So crashes can be debuged in release mode
  #add_custom_command(TARGET Engine2D POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:Engine2D>)
endif()

# Include CMakeRC
include(CMakeRC.cmake)
cmrc_add_resource_library(assets NAMESPACE engineResources ${CMAKE_SOURCE_DIR}/Assets)

# Include cpptrace to be able to print stack traces
include(FetchContent)
FetchContent_Declare(
    cpptrace
    GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
    GIT_TAG        v0.7.3 # <HASH or TAG>
)
FetchContent_MakeAvailable(cpptrace)

set_target_properties(Engine2D PROPERTIES
    OUTPUT_NAME Engine2D-0.2.6.1
    PREFIX ""
)

target_link_libraries(Engine2D PUBLIC
    OpenGL::GL
    glfw
    GLEW::GLEW
    glm::glm
    assets
    cpptrace::cpptrace
)

if (APPLE)
  target_link_libraries(Engine2D PUBLIC
      "-framework CoreFoundation"
      "-framework IOKit"
      "-framework Cocoa"
  )
elseif (UNIX)
  message(STATUS "Linux build is not fully supported yet, there could be some errors.")
elseif (WIN32)
  message(STATUS "Window build is not fully supported yet, there could be some errors.")
else ()
  message(STATUS "Unknown operating system, please configure the library linking.")
endif ()

target_include_directories(Engine2D PUBLIC
    EngineFiles
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${cpptrace_SOURCE_DIR}/include
)