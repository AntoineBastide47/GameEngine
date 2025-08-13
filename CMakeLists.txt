cmake_minimum_required(VERSION 3.29)
project(GameEngine)

option(BUILD_ENGINE "Build the engine library" ON)
option(BUILD_EDITOR "Build the editor application" ON)

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
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
add_subdirectory(vendor/glfw)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# === GLM ===
add_subdirectory(vendor/glm)

# === CMakeRC ===
include(vendor/CMakeRC/CMakeRC.cmake)

# === Cpptrace ===
add_subdirectory(vendor/cpptrace)
add_library(cpptrace STATIC vendor/cpptrace/src/cpptrace.cpp)
target_include_directories(cpptrace PUBLIC vendor/cpptrace/include vendor/cpptrace/src)

# --------------------------
# Build Configs
# --------------------------

if (BUILD_ENGINE)
    add_subdirectory(Engine)
endif ()

if (BUILD_EDITOR)
    add_subdirectory(Editor)
endif ()