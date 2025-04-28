########################################################################################################################
#                                                                                                                      #
#                                   Engine Required CMake code, Please do not modify                                   #
#                                                                                                                      #
########################################################################################################################

cmake_minimum_required(VERSION 3.29)
project({{PROJECT_NAME}})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(ENGINE_LOCATION {{ENGINE_LOCATION}})

# --------------------------
# Dependencies
# --------------------------

# === GLEW  ===
set(GLEW_USE_STATIC_LIBS ON)
find_package(GLEW REQUIRED)

# === GLFW ===
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(
    ${ENGINE_LOCATION}/vendor/glfw
    ${CMAKE_BINARY_DIR}/glfw
)

# === CMakeRC ===
include(${ENGINE_LOCATION}/vendor/CMakeRC/CMakeRC.cmake)

# === Cpptrace ===
add_subdirectory(
    ${ENGINE_LOCATION}/vendor/cpptrace
    ${CMAKE_BINARY_DIR}/cpptrace
)
add_library(cpptrace STATIC ${ENGINE_LOCATION}/vendor/cpptrace/src/cpptrace.cpp)
target_include_directories(cpptrace PUBLIC ${ENGINE_LOCATION}/vendor/cpptrace/include ${ENGINE_LOCATION}/vendor/cpptrace/src)

# --------------------------
# Engine Setup
# --------------------------

if (APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "16.0")
  set(ENGINE_PLATFORM_LIBS "-framework CoreFoundation -framework IOKit -framework Cocoa")
elseif (LINUX)
  message(WARNING "Linux build is not fully supported yet.")
elseif (WIN32)
  message(WARNING "Windows build is not fully supported yet.")
endif ()

file(GLOB ENGINE_LIB ${ENGINE_LOCATION}/include/*-[0-9]*.[0-9]*.[0-9]*.*)
if (BUILD_TYPE STREQUAL "debug")
  set(CMAKE_BUILD_TYPE "Debug")
  list(FILTER ENGINE_LIB INCLUDE REGEX "-[0-9]+\\.[0-9]+\\.[0-9]+d\\.[^.]+$")
elseif (BUILD_TYPE STREQUAL "profile")
  set(CMAKE_BUILD_TYPE "Debug")
  list(FILTER ENGINE_LIB INCLUDE REGEX "-[0-9]+\\.[0-9]+\\.[0-9]+dp\\.[^.]+$")
elseif (BUILD_TYPE STREQUAL "release")
  set(CMAKE_BUILD_TYPE "Release")
  list(FILTER ENGINE_LIB INCLUDE REGEX "-[0-9]+\\.[0-9]+\\.[0-9]+\\.[^.]+$")
else()
  set(CMAKE_BUILD_TYPE "Debug")
  list(FILTER ENGINE_LIB INCLUDE REGEX "-[0-9]+\\.[0-9]+\\.[0-9]+d\\.[^.]+$")
endif ()

list(LENGTH ENGINE_LIB ENGINE_COUNT)
if (ENGINE_COUNT EQUAL 0)
  message(FATAL_ERROR "Engine static library not found in EngineInclude.")
elseif (ENGINE_COUNT GREATER 1)
  message(WARNING "Multiple engine libraries found in EngineInclude.")
endif ()

# --------------------------
# Shader Embedding
# --------------------------

file(GLOB_RECURSE SHADERS Engine/Shaders/*)
cmrc_add_resource_library(shaders NAMESPACE engine_resources ${SHADERS})

# --------------------------
# Executable Definition
# --------------------------

add_executable(${PROJECT_NAME} main.cpp
    Game/include/{{PROJECT_NAME}}.hpp
    Game/src/{{PROJECT_NAME}}.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${ENGINE_LOCATION}/include Game/include)
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${ENGINE_PLATFORM_LIBS}
    ${ENGINE_LIB}
    GLEW::GLEW
    glfw
    cpptrace::cpptrace
    shaders
)

# --------------------------
# Build Configs
# --------------------------

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  target_compile_options(${PROJECT_NAME} PRIVATE
      -O3 -DNDEBUG -finline-functions -fomit-frame-pointer
      -ffunction-sections -fdata-sections
  )
  target_link_options(${PROJECT_NAME} PRIVATE -Wl,-O2)
  set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${PROJECT_NAME}>)
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set(SANITIZER_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
    set(EXTRA_DEBUG_FLAGS "-g -O1")
    set(ASAN_OPTIONS_FLAGS "-fsanitize-recover=address -fsanitize=undefined -fsanitize-address-use-after-scope")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS} ${EXTRA_DEBUG_FLAGS} ${ASAN_OPTIONS_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SANITIZER_FLAGS} ${EXTRA_DEBUG_FLAGS} ${ASAN_OPTIONS_FLAGS}")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} ${SANITIZER_FLAGS} ${EXTRA_DEBUG_FLAGS} ${ASAN_OPTIONS_FLAGS}")
    add_compile_definitions(ASAN_OPTIONS="verbosity=1:halt_on_error=1:detect_leaks=1:symbolize=1")
  endif ()
endif ()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rename
    $<TARGET_FILE:${PROJECT_NAME}>
    ${CMAKE_CURRENT_BINARY_DIR}/../$<TARGET_FILE_NAME:${PROJECT_NAME}>
)

# --------------------------
# Macros
# --------------------------

target_compile_definitions(${PROJECT_NAME} PUBLIC
    $<$<CONFIG:Debug>:GAME_DEBUG=1>
)

########################################################################################################################
#                                                                                                                      #
#                                           Game CMake code, modify as needed                                          #
#                                                                                                                      #
########################################################################################################################

# --------------------------
# Assets Embedding
# --------------------------

# Embed the assets into the executable
file(GLOB_RECURSE ASSET_FILES ${CMAKE_SOURCE_DIR}/Game/Assets/*)
cmrc_add_resource_library(assets NAMESPACE resources ${ASSET_FILES})

# --------------------------
# Game Dependencies
# --------------------------

# Link user libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
    assets
)

# Include user include directories
target_include_directories(${PROJECT_NAME} PUBLIC

)