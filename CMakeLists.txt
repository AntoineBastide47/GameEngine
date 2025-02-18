cmake_minimum_required(VERSION 3.29)
project(GameEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# Create the Engine2D as a static library
add_library(Engine2D SHARED
    EngineInclude/Engine/Macros.h
    EngineInclude/Engine2D/Rendering/stb_image.h
    EngineInclude/Engine/RenderingHeaders.h
    EngineInclude/Engine2D/Types/Vector2.h
    EngineCode/Engine2D/Game2D.cpp
    EngineInclude/Engine2D/Game2D.h
    EngineCode/Engine2D/Rendering/Shader.cpp
    EngineInclude/Engine2D/Rendering/Shader.h
    EngineCode/Engine2D/Rendering/Texture2D.cpp
    EngineInclude/Engine2D/Rendering/Texture2D.h
    EngineCode/Engine2D/ResourceManager.cpp
    EngineInclude/Engine2D/ResourceManager.h
    EngineCode/Engine2D/Rendering/SpriteRenderer.cpp
    EngineInclude/Engine2D/Rendering/SpriteRenderer.h
    EngineCode/Engine2D/Transform2D.cpp
    EngineInclude/Engine2D/Transform2D.h
    EngineCode/Engine2D/Entity2D.cpp
    EngineInclude/Engine2D/Entity2D.h
    EngineCode/Engine/Log.cpp
    EngineInclude/Engine/Log.h
    EngineInclude/Engine/Event.h
    EngineCode/Engine/Input/Keyboard.cpp
    EngineInclude/Engine/Input/Keyboard.h
    EngineCode/Engine/Input/Mouse.cpp
    EngineInclude/Engine/Input/Mouse.h
    EngineCode/Engine/Input/Gamepad.cpp
    EngineInclude/Engine/Input/Gamepad.h
    EngineInclude/Engine/Input/InputContexts.h
    EngineCode/Engine2D/Component2D.cpp
    EngineInclude/Engine2D/Component2D.h
    EngineCode/Engine2D/Physics/Collisions.cpp
    EngineInclude/Engine2D/Physics/Collisions.h
    EngineCode/Engine2D/Physics/Physics2D.cpp
    EngineInclude/Engine2D/Physics/Physics2D.h
    EngineCode/Engine2D/Physics/Rigidbody2D.cpp
    EngineInclude/Engine2D/Physics/Rigidbody2D.h
    EngineCode/Engine2D/Physics/CollisionManifold.cpp
    EngineInclude/Engine2D/Physics/CollisionManifold.h
    EngineCode/Engine/Types/float01.cpp
    EngineInclude/Engine/Types/float01.h
    EngineCode/Engine2D/Physics/CollisionGrid.cpp
    EngineInclude/Engine2D/Physics/CollisionGrid.h
    EngineCode/Engine/Settings.cpp
    EngineInclude/Engine/Settings.h
    EngineCode/Engine2D/Physics/Collider2D.cpp
    EngineInclude/Engine2D/Physics/Collider2D.h
    EngineInclude/Engine2D/Physics/CollisionPair.h
    EngineCode/Engine2D/ParticleSystem/ParticleSystem2D.cpp
    EngineInclude/Engine2D/ParticleSystem/ParticleSystem2D.h
    EngineCode/Engine2D/ParticleSystem/ParticleSystemRenderer2D.cpp
    EngineInclude/Engine2D/ParticleSystem/ParticleSystemRenderer2D.h
    EngineCode/Engine2D/Rendering/ShapeRenderer.cpp
    EngineInclude/Engine2D/Rendering/ShapeRenderer.h
    EngineCode/Engine2D/Behaviour.cpp
    EngineInclude/Engine2D/Behaviour.h
)

set_target_properties(Engine2D PROPERTIES OUTPUT_NAME Engine2D-0.5.0 PREFIX "")

# Include CMakeRC
include(CMakeRC.cmake)
cmrc_add_resource_library(assets NAMESPACE engineResources ${CMAKE_SOURCE_DIR}/Assets)
target_link_libraries(Engine2D PUBLIC assets)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw)

# GLM
add_subdirectory(external/glm)
target_include_directories(glm INTERFACE external/glm)

# Cpptrace
set(CPPTRACE_SOURCE_DIR external/cpptrace)
add_subdirectory(${CPPTRACE_SOURCE_DIR})

# Include all other libraries
target_link_libraries(Engine2D PRIVATE
    OpenGL::GL
    GLEW::GLEW
    glfw
    glm
    cpptrace::cpptrace
)

if (APPLE)
  target_link_libraries(Engine2D PRIVATE
      "-framework CoreFoundation"
      "-framework IOKit"
      "-framework Cocoa"
  )
elseif (UNIX)
  message(STATUS "Linux build is not fully supported yet, there could be some errors.")
elseif (WIN32)
  message(STATUS "Window build is not fully supported yet, there could be some errors.")
endif ()

set(ENGINE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/EngineInclude)
target_include_directories(Engine2D PUBLIC ${ENGINE_INCLUDE_DIR})
target_include_directories(Engine2D PRIVATE
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${cpptrace_SOURCE_DIR}/include
)

target_compile_options(Engine2D PUBLIC -fexperimental-library)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  target_compile_options(Engine2D PRIVATE
      -O3                     # General optimization
      -DNDEBUG                # Disable assertions
      -finline-functions      # Enable inlining
      -flto                   # Link-Time Optimization
      -fomit-frame-pointer    # Free up a register
      -ffunction-sections     # Enable section-based optimization
      -fdata-sections         # Enable data section optimization
  )
  target_link_options(Engine2D PRIVATE -Wl,-O2)
  set_target_properties(Engine2D PROPERTIES POSITION_INDEPENDENT_CODE ON)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
  #add_custom_command(TARGET Engine2D POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:Engine2D>)

  target_compile_definitions(Engine2D PUBLIC DEBUG_BUILD=0)
else ()
  target_compile_definitions(Engine2D PUBLIC DEBUG_BUILD=1)
endif ()