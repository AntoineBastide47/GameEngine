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
if (NOT DEFINED PROJECT_BUILD_TYPE)
    set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
else ()
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
endif ()
add_subdirectory(
    ${ENGINE_LOCATION}/vendor/glfw
    ${CMAKE_BINARY_DIR}/glfw
)
if (NOT DEFINED PROJECT_BUILD_TYPE)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
endif ()

# === CMakeRC ===
include(${ENGINE_LOCATION}/vendor/CMakeRC/CMakeRC.cmake)

# === Cpptrace ===
add_subdirectory(
    ${ENGINE_LOCATION}/vendor/cpptrace
    ${CMAKE_BINARY_DIR}/cpptrace
)
add_library(cpptrace STATIC ${ENGINE_LOCATION}/vendor/cpptrace/src/cpptrace.cpp)
target_include_directories(cpptrace PUBLIC
    ${ENGINE_LOCATION}/vendor/cpptrace/include
    ${ENGINE_LOCATION}/vendor/cpptrace/src
)

# === GLM ===
add_subdirectory(
    ${ENGINE_LOCATION}/vendor/glm
    ${CMAKE_BINARY_DIR}/glm
)

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

if (NOT DEFINED PROJECT_BUILD_TYPE)
    set(PROJECT_BUILD_TYPE "static")
endif ()
file(GLOB ENGINE_LIB ${ENGINE_LOCATION}/lib/${PROJECT_BUILD_TYPE}/*)

if (BUILD_TYPE STREQUAL "profile")
    set(CMAKE_BUILD_TYPE "Debug")
    list(FILTER ENGINE_LIB INCLUDE REGEX "-[0-9]+\\.[0-9]+\\.[0-9]+dp\\.[^.]+$")
elseif (BUILD_TYPE STREQUAL "release")
    set(CMAKE_BUILD_TYPE "Release")
    list(FILTER ENGINE_LIB INCLUDE REGEX "-[0-9]+\\.[0-9]+\\.[0-9]+\\.[^.]+$")
else ()
    set(CMAKE_BUILD_TYPE "Debug")
    list(FILTER ENGINE_LIB INCLUDE REGEX "-[0-9]+\\.[0-9]+\\.[0-9]+d\\.[^.]+$")
endif ()

list(LENGTH ENGINE_LIB ENGINE_COUNT)
if (ENGINE_COUNT EQUAL 0)
    message(FATAL_ERROR "Engine static library not found.")
elseif (ENGINE_COUNT GREATER 1)
    message(WARNING "Multiple engine libraries found.")
endif ()

set(DEPENDENCIES GLEW::GLEW glfw cpptrace::cpptrace glm)

# --------------------------
# Editor Setup
# --------------------------

set(EDITOR_INCLUDE "")
set(EDITOR_FILES "")

if (PROJECT_BUILD_TYPE STREQUAL "shared")
    set(EDITOR_INCLUDE ${ENGINE_LOCATION}/Editor Engine/include)
    list(APPEND EDITOR_FILES
        Engine/src/Plugin.cpp
        Engine/include/Plugin.hpp
    )
endif ()

# --------------------------
# Header-Forge Setup
# --------------------------

list(APPEND HEADER_FORGE_COMPILER_ARGS
    "-I../${ENGINE_LOCATION}/Engine "
    "-I../${ENGINE_LOCATION}/vendor/glfw/include "
    "-I../${ENGINE_LOCATION}/vendor/glm "
    "-I../${ENGINE_LOCATION}/vendor/cpptrace/include "
    "-I./_cmrc/include "
)

add_custom_target(HeaderForge
    COMMAND ${CMAKE_COMMAND} -E echo "Running reflection engine..."
    COMMAND ../header-forge --parse ../Game/include --compilerArgs ${HEADER_FORGE_COMPILER_ARGS}
    COMMENT "Engine header-forge step"
)

add_dependencies(HeaderForge ${DEPENDENCIES})

# --------------------------
# Executable Definition
# --------------------------

add_executable(${PROJECT_NAME} main.cpp ${EDITOR_FILES}
    Game/include/{{PROJECT_NAME}}.hpp
    Game/src/{{PROJECT_NAME}}.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${ENGINE_LOCATION}/Engine ${EDITOR_INCLUDE}
    Game/include
)
target_link_libraries(${PROJECT_NAME} PUBLIC
    ${ENGINE_PLATFORM_LIBS} ${ENGINE_LIB} ${DEPENDENCIES}
)
add_dependencies(${PROJECT_NAME} HeaderForge)

# --------------------------
# Shader Embedding
# --------------------------

# Copy the shaders
set(ENGINE_TEMP_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/Temp")
file(MAKE_DIRECTORY "${ENGINE_TEMP_LOCATION}/Engine")
file(COPY "${ENGINE_LOCATION}/Engine/Shaders" DESTINATION "${ENGINE_TEMP_LOCATION}/Engine")

# Embed the shaders
file(GLOB_RECURSE SHADERS "${ENGINE_TEMP_LOCATION}/Engine/Shaders/*.glsl")
cmrc_add_resource_library(shaders
    NAMESPACE engine_resources
    WHENCE "${ENGINE_TEMP_LOCATION}/Engine/Shaders"
    PREFIX "Engine/Shaders"
    ${SHADERS}
)
target_link_libraries(${PROJECT_NAME} PRIVATE shaders)

# Remove the copied shaders
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E remove_directory "${ENGINE_TEMP_LOCATION}")

# --------------------------
# Build Configs
# --------------------------

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -O3 -DNDEBUG -finline-functions -fomit-frame-pointer
        -ffunction-sections -fdata-sections
        -fno-fast-math -fno-strict-aliasing -frounding-math
    )
    target_link_options(${PROJECT_NAME} PRIVATE -Wl,-O2)
    set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${PROJECT_NAME}>)
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        set(SANITIZER_FLAGS "-fsanitize=address,undefined")
        set(DEBUG_FLAGS -g -fno-omit-frame-pointer -fsanitize-address-use-after-scope)

        # Apply compile and link flags properly
        target_compile_options(${PROJECT_NAME} PRIVATE ${SANITIZER_FLAGS} ${DEBUG_FLAGS})
        target_link_options(${PROJECT_NAME} PRIVATE ${SANITIZER_FLAGS} ${DEBUG_FLAGS})

        # ASAN runtime options as a macro
        add_compile_definitions(ASAN_OPTIONS="verbosity=1:halt_on_error=1:detect_leaks=1:symbolize=1")
    endif ()
endif ()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rename $<TARGET_FILE:${PROJECT_NAME}> ${CMAKE_CURRENT_BINARY_DIR}/../$<TARGET_FILE_NAME:${PROJECT_NAME}>
)

# --------------------------
# Macros
# --------------------------

target_compile_definitions(${PROJECT_NAME} PUBLIC
    $<$<CONFIG:Debug>:GAME_DEBUG=1>
    MULTI_THREAD=1
    ENGINE_EDITOR=$<BOOL:$<STREQUAL:${PROJECT_BUILD_TYPE},shared>>
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
list(FILTER ASSET_FILES EXCLUDE REGEX "\\.DS_Store$")
cmrc_add_resource_library(assets NAMESPACE assets ${ASSET_FILES})

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