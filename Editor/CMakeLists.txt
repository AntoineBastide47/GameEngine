cmake_minimum_required(VERSION 3.29)
project(EngineEditor VERSION 0.5.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_NAME "EditorExe")
set(CMAKE_BINARY_DIR ./build)
set(CMAKE_BUILD_TYPE "Debug")

# Find engine library
file(GLOB ENGINE_LIB ${CMAKE_SOURCE_DIR}/SDK/lib/shared/*)
list(FILTER ENGINE_LIB INCLUDE REGEX "-[0-9]+\\.[0-9]+\\.[0-9]+d\\.[^.]+$")

list(LENGTH ENGINE_LIB ENGINE_COUNT)
if (ENGINE_COUNT EQUAL 0)
    message(FATAL_ERROR "Engine shared library not found in SDK.")
elseif (ENGINE_COUNT GREATER 1)
    message(WARNING "Multiple engine libraries found in SDK.")
endif ()

add_executable(${EXECUTABLE_NAME}
    src/main.cpp
    src/Window.cpp
    include/Window.hpp
    src/LevelEditor.cpp
    include/LevelEditor.hpp
    include/GamePlugin.hpp
    src/ProjectLoader.cpp
    include/ProjectLoader.hpp
    src/Panels/SceneViewport.cpp
    include/Panels/SceneViewport.hpp
    src/Panels/SceneViewport.cpp
    include/Panels/SceneViewport.hpp
    src/Panels/SceneHierarchy.cpp
    include/Panels/SceneHierarchy.hpp
    include/History/EditorCommand.hpp
    src/History/CommandHistory.cpp
    include/History/CommandHistory.hpp
    src/History/Commands/CreateEntityCommand.cpp
    include/History/Commands/CreateEntityCommand.hpp
    src/History/Commands/DeleteEntityCommand.cpp
    include/History/Commands/DeleteEntityCommand.hpp
    src/History/Commands/SelectEntityCommand.cpp
    include/History/Commands/SelectEntityCommand.hpp
    src/Panels/EntityInspector.cpp
    include/Panels/EntityInspector.hpp
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    OUTPUT_NAME ${EXECUTABLE_NAME}
    PREFIX ""
)

target_include_directories(${EXECUTABLE_NAME} PUBLIC
    include ${CMAKE_SOURCE_DIR}/SDK/Engine
)

# === OS Specific Requirements ===
if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "15.0")
    target_link_libraries(${EXECUTABLE_NAME} PUBLIC
        "-framework CoreFoundation"
        "-framework IOKit"
        "-framework Cocoa"
    )
elseif (LINUX)
    message(WARNING "Linux build is not fully supported yet.")
elseif (WIN32)
    message(WARNING "Windows build is not fully supported yet.")
endif ()

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    ${ENGINE_LIB} OpenGL::GL GLEW::GLEW glfw glm cmrc-base imgui cpptrace::cpptrace
)

target_include_directories(${EXECUTABLE_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/vendor/glfw/include
    ${CMAKE_SOURCE_DIR}/vendor/glew/include
)

# --------------------------
# Build Configs
# --------------------------

include(../Engine/CMake/BuildConfig.cmake)
setup_build_config(${EXECUTABLE_NAME})

add_custom_command(
    TARGET ${EXECUTABLE_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rename $<TARGET_FILE:${EXECUTABLE_NAME}> ${CMAKE_CURRENT_BINARY_DIR}/../../../$<TARGET_FILE_NAME:${EXECUTABLE_NAME}>
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# --------------------------
# Macros
# --------------------------

target_compile_definitions(${EXECUTABLE_NAME} PUBLIC
    ENGINE_EDITOR=1
    MULTI_THREAD=${MULTI_THREAD}
)

include(../Engine/CMake/PlatformDetection.cmake)
setup_platform_detection(${EXECUTABLE_NAME})