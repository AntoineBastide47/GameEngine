cmake_minimum_required(VERSION 3.29)
project(GameEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLM REQUIRED)

# Include cpptrace to be able to print stack traces
include(FetchContent)
FetchContent_Declare(
    cpptrace
    GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
    GIT_TAG v0.7.3 # <HASH or TAG>
)
FetchContent_MakeAvailable(cpptrace)

# Create the Engine2D as a static library
add_library(Engine2D SHARED
    Engine/Include/Common/Macros.h
    Engine/Include/Engine2D/Rendering/stb_image.h
    Engine/Include/Common/RenderingHeaders.h
    EngineCode/Engine2D/Game2D.cpp
    Engine/Include/Engine2D/Game2D.h
    EngineCode/Engine2D/Rendering/Shader.cpp
    Engine/Include/Engine2D/Rendering/Shader.h
    EngineCode/Engine2D/Rendering/Texture2D.cpp
    Engine/Include/Engine2D/Rendering/Texture2D.h
    EngineCode/Engine2D/ResourceManager.cpp
    Engine/Include/Engine2D/ResourceManager.h
    EngineCode/Engine2D/Rendering/SpriteRenderer.cpp
    Engine/Include/Engine2D/Rendering/SpriteRenderer.h
    Engine/Include/Engine2D/Types/Vector2.h
    EngineCode/Engine2D/Transform2D.cpp
    Engine/Include/Engine2D/Transform2D.h
    EngineCode/Engine2D/Entity2D.cpp
    Engine/Include/Engine2D/Entity2D.h
    EngineCode/Common/Log.cpp
    Engine/Include/Common/Log.h
    Engine/Include/Common/Event.h
    EngineCode/Input/Keyboard.cpp
    Engine/Include/Input/Keyboard.h
    EngineCode/Input/Mouse.cpp
    Engine/Include/Input/Mouse.h
    EngineCode/Input/Gamepad.cpp
    Engine/Include/Input/Gamepad.h
    Engine/Include/Input/InputContexts.h
    EngineCode/Engine2D/Component2D.cpp
    Engine/Include/Engine2D/Component2D.h
    EngineCode/Engine2D/Physics/Collisions.cpp
    Engine/Include/Engine2D/Physics/Collisions.h
    EngineCode/Engine2D/Physics/Physics2D.cpp
    Engine/Include/Engine2D/Physics/Physics2D.h
    EngineCode/Engine2D/Physics/Rigidbody2D.cpp
    Engine/Include/Engine2D/Physics/Rigidbody2D.h
    EngineCode/Engine2D/Physics/CollisionManifold.cpp
    Engine/Include/Engine2D/Physics/CollisionManifold.h
    EngineCode/Types/float01.cpp
    Engine/Include/Types/float01.h
    EngineCode/Engine2D/Physics/CollisionGrid.cpp
    Engine/Include/Engine2D/Physics/CollisionGrid.h
    EngineCode/Common/Settings.cpp
    Engine/Include/Common/Settings.h
    EngineCode/Engine2D/Physics/Collider2D.cpp
    Engine/Include/Engine2D/Physics/Collider2D.h
    Engine/Include/Engine2D/Physics/CollisionPair.h
    EngineCode/Engine2D/ParticleSystem/ParticleSystem2D.cpp
    Engine/Include/Engine2D/ParticleSystem/ParticleSystem2D.h
    EngineCode/Engine2D/ParticleSystem/ParticleSystemRenderer2D.cpp
    Engine/Include/Engine2D/ParticleSystem/ParticleSystemRenderer2D.h
    EngineCode/Engine2D/Rendering/ShapeRenderer.cpp
    Engine/Include/Engine2D/Rendering/ShapeRenderer.h
)

set_target_properties(Engine2D PROPERTIES OUTPUT_NAME Engine2D-0.4.1 PREFIX "")

# Include CMakeRC
include(CMakeRC.cmake)
cmrc_add_resource_library(assets NAMESPACE engineResources ${CMAKE_SOURCE_DIR}/Assets)

target_link_libraries(Engine2D PUBLIC assets)
target_link_libraries(Engine2D PRIVATE
    OpenGL::GL
    glfw
    GLEW::GLEW
    glm::glm
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

set(ENGINE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Engine/Include)
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