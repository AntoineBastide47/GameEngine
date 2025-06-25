cmake_minimum_required(VERSION 3.29)
project(GameEngine VERSION 0.8.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Define the engine library
add_library(Engine2D STATIC
    include/Engine2D/Rendering/stb_image.hpp
    include/Engine/RenderingHeaders.hpp
    include/Engine2D/Types/Vector2.hpp
    src/Engine2D/Game2D.cpp
    include/Engine2D/Game2D.hpp
    src/Engine/Rendering/Shader.cpp
    include/Engine/Rendering/Shader.hpp
    src/Engine/Rendering/Texture.cpp
    include/Engine/Rendering/Texture.hpp
    src/Engine/ResourceManager.cpp
    include/Engine/ResourceManager.hpp
    src/Engine2D/Rendering/Renderer2D.cpp
    include/Engine2D/Rendering/Renderer2D.hpp
    src/Engine2D/Transform2D.cpp
    include/Engine2D/Transform2D.hpp
    src/Engine2D/Entity2D.cpp
    include/Engine2D/Entity2D.hpp
    src/Engine/Log.cpp
    include/Engine/Log.hpp
    include/Engine/Event.hpp
    src/Engine/Input/Keyboard.cpp
    include/Engine/Input/Keyboard.hpp
    src/Engine/Input/Mouse.cpp
    include/Engine/Input/Mouse.hpp
    src/Engine/Input/Gamepad.cpp
    include/Engine/Input/Gamepad.hpp
    include/Engine/Input/InputContexts.hpp
    src/Engine2D/Component2D.cpp
    include/Engine2D/Component2D.hpp
    src/Engine2D/Physics/Collisions.cpp
    include/Engine2D/Physics/Collisions.hpp
    src/Engine2D/Physics/Physics2D.cpp
    include/Engine2D/Physics/Physics2D.hpp
    src/Engine2D/Physics/Rigidbody2D.cpp
    include/Engine2D/Physics/Rigidbody2D.hpp
    src/Engine2D/Physics/CollisionManifold.cpp
    include/Engine2D/Physics/CollisionManifold.hpp
    src/Engine/Types/float01.cpp
    include/Engine/Types/float01.hpp
    src/Engine2D/Physics/CollisionGrid.cpp
    include/Engine2D/Physics/CollisionGrid.hpp
    src/Engine/Settings.cpp
    include/Engine/Settings.hpp
    src/Engine2D/Physics/Collider2D.cpp
    include/Engine2D/Physics/Collider2D.hpp
    src/Engine2D/ParticleSystem/ParticleSystem2D.cpp
    include/Engine2D/ParticleSystem/ParticleSystem2D.hpp
    src/Engine2D/ParticleSystem/ParticleSystemRegistry2D.cpp
    include/Engine2D/ParticleSystem/ParticleSystemRegistry2D.hpp
    src/Engine2D/Behaviour.cpp
    include/Engine2D/Behaviour.hpp
    include/Engine/Macros/Utils.hpp
    include/Engine/Macros/PlatformDetection.hpp
    include/Engine/Macros/Assert.hpp
    include/Engine2D/Rendering/Sprite.hpp
    src/Engine2D/Rendering/SpriteRenderer.cpp
    include/Engine2D/Rendering/SpriteRenderer.hpp
    src/Engine2D/Rendering/Camera2D.cpp
    include/Engine2D/Rendering/Camera2D.hpp
    src/Engine/Profiling/Timer.cpp
    include/Engine/Profiling/Timer.hpp
    src/Engine/Profiling/Instrumentor.cpp
    include/Engine/Profiling/Instrumentor.hpp
    include/Engine/Macros/Profiling.hpp
    src/Engine2D/Animation/Animation2D.cpp
    include/Engine2D/Animation/Animation2D.hpp
    src/Engine2D/Animation/Animator2D.cpp
    include/Engine2D/Animation/Animator2D.hpp
    src/Engine2D/Animation/AnimationSystem.cpp
    include/Engine2D/Animation/AnimationSystem.hpp
    src/Engine/Rendering/ShaderPreProcessor.cpp
    include/Engine/Rendering/ShaderPreProcessor.hpp
    src/Engine2D/Rendering/Renderable2D.cpp
    include/Engine2D/Rendering/Renderable2D.hpp
    include/Engine/Reflection/Concepts.hpp
    include/Engine/Reflection/Save.hpp
    include/Engine/Reflection/Reflectable.hpp
    include/Engine/Reflection/Serializer.hpp
)

set_target_properties(Engine2D PROPERTIES
    OUTPUT_NAME Engine2D-${PROJECT_VERSION}$<IF:$<STREQUAL:$<CONFIG>,Debug>,d,>$<IF:$<BOOL:${PROFILE}>,p,>
    PREFIX ""
)
target_include_directories(Engine2D PUBLIC include)

# --------------------------
# Dependencies
# --------------------------

# === GLEW + OpenGL ===
find_package(OpenGL REQUIRED)
set(GLEW_USE_STATIC_LIBS ON)
find_package(GLEW REQUIRED)

# === GLFW ===
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/glfw)

# === GLM ===
add_subdirectory(vendor/glm)

# === CMakeRC ===
include(vendor/CMakeRC/CMakeRC.cmake)
cmrc_add_resource_library(cmrc NAMESPACE engineResources Assets)

# === Cpptrace ===
add_subdirectory(vendor/cpptrace)
add_library(cpptrace STATIC vendor/cpptrace/src/cpptrace.cpp)
target_include_directories(cpptrace PUBLIC vendor/cpptrace/include vendor/cpptrace/src)

# === OS Specific Requirements ===
if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "15.0")
    target_link_libraries(Engine2D PUBLIC
        "-framework CoreFoundation"
        "-framework IOKit"
        "-framework Cocoa"
    )
elseif (LINUX)
    message(WARNING "Linux build is not fully supported yet.")
elseif (WIN32)
    message(WARNING "Windows build is not fully supported yet.")
endif ()

target_link_libraries(Engine2D PRIVATE
    OpenGL::GL GLEW::GLEW glfw glm-header-only
    cmrc cpptrace::cpptrace imgui
)

target_link_directories(Engine2D PRIVATE
    vendor/imgui
)

# --------------------------
# Build Configs
# --------------------------

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(Engine2D PRIVATE
        -O3 -DNDEBUG -finline-functions -fomit-frame-pointer
        -ffunction-sections -fdata-sections
        -fno-fast-math -fno-strict-aliasing -frounding-math
    )
    target_link_options(Engine2D PRIVATE -Wl,-O2)
    set_target_properties(Engine2D PROPERTIES POSITION_INDEPENDENT_CODE ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    add_custom_command(TARGET Engine2D POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:Engine2D>)
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        set(SANITIZER_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
        set(ASAN_OPTIONS_FLAGS "-fsanitize-recover=address -fsanitize=undefined -fsanitize-address-use-after-scope")
        set(DEBUG_FLAGS "-g")

        add_compile_definitions(ASAN_OPTIONS="verbosity=1:halt_on_error=1:detect_leaks=1:symbolize=1")

        # Apply flags to all targets
        set(ALL_SANITIZER_FLAGS "${SANITIZER_FLAGS} ${ASAN_OPTIONS_FLAGS} ${DEBUG_FLAGS}")

        # Function to apply sanitizer flags to all targets
        function(enable_sanitizers target)
            target_compile_options(${target} PRIVATE ${ALL_SANITIZER_FLAGS})
            target_link_options(${target} PRIVATE ${ALL_SANITIZER_FLAGS})
        endfunction()
    endif ()
endif ()

add_custom_command(TARGET Engine2D POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rename
    $<TARGET_FILE:Engine2D>
    ${CMAKE_CURRENT_BINARY_DIR}/../../include/$<TARGET_FILE_NAME:Engine2D>
)

# --------------------------
# Macros
# --------------------------

target_compile_definitions(Engine2D PUBLIC
    $<$<CONFIG:Debug>:ENGINE_DEBUG=1>
    $<$<BOOL:${PROFILE}>:ENGINE_PROFILING=1>
    MULTI_THREAD=1 # Set this to 0 if you have mutex errors when modifying the engine
)