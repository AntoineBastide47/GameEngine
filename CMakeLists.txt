cmake_minimum_required(VERSION 3.29)
project(GameEngine)

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
cmrc_add_resource_library(cmrc NAMESPACE engineResources Engine/Assets)

# === Cpptrace ===
add_subdirectory(vendor/cpptrace)
add_library(cpptrace STATIC vendor/cpptrace/src/cpptrace.cpp)
target_include_directories(cpptrace PUBLIC vendor/cpptrace/include vendor/cpptrace/src)

# --------------------------
# Build Configs
# --------------------------

option(BUILD_ENGINE "Build the engine library" OFF)
if (BUILD_ENGINE)
    add_subdirectory(Engine)
endif ()

option(BUILD_EDITOR "Build the editor application" OFF)
if (BUILD_EDITOR)
    add_subdirectory(Editor)
endif ()