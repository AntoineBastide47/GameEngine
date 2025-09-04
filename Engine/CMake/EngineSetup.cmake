########################################################################################################################
#                                                                                                                      #
#                                   Engine Required CMake code, Please do not modify                                   #
#                                                                                                                      #
########################################################################################################################

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ENGINE_LOCATION should be set before including this file
if (NOT DEFINED ENGINE_LOCATION)
    message(FATAL_ERROR "ENGINE_LOCATION must be defined before including EngineSetup.cmake")
endif ()

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
set(BUILD_SHARED_LIBS $<IF:$<BOOL:${PROJECT_BUILD_TYPE}>,OFF,ON> CACHE BOOL "" FORCE)
add_subdirectory(
    ${ENGINE_LOCATION}/vendor/glfw
    ${CMAKE_BINARY_DIR}/glfw
)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

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

# === ImGui ===
include(${ENGINE_LOCATION}/CMake/ImGui.cmake)
setup_imgui(${ENGINE_LOCATION})

# --------------------------
# Platform Detection
# --------------------------

if (APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "16.0")
    set(ENGINE_PLATFORM_LIBS "-framework CoreFoundation -framework IOKit -framework Cocoa")
elseif (LINUX)
    message(WARNING "Linux build is not fully supported yet.")
elseif (WIN32)
    message(WARNING "Windows build is not fully supported yet.")
endif ()

# --------------------------
# Engine Library Setup
# --------------------------

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

set(ENGINE_DEPENDENCIES GLEW::GLEW glfw cpptrace::cpptrace glm imgui)

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
    "-I../${ENGINE_LOCATION}/vendor/imgui "
    "-I./_cmrc/include "
)

# Function to create HeaderForge target if it doesn't exist
function(setup_header_forge)
    if (NOT TARGET HeaderForge)
        add_custom_target(HeaderForge
            COMMAND ${CMAKE_COMMAND} -E echo "Running reflection engine..."
            COMMAND ../header-forge --parse ../Game/include --compilerArgs ${HEADER_FORGE_COMPILER_ARGS}
            COMMENT "Engine header-forge step"
        )
        add_dependencies(HeaderForge ${ENGINE_DEPENDENCIES})
    endif ()
endfunction()

# --------------------------
# Engine Setup Function
# --------------------------

function(setup_engine_target TARGET_NAME)
    # Add engine includes
    target_include_directories(${TARGET_NAME} PRIVATE ${ENGINE_LOCATION}/Engine ${EDITOR_INCLUDE})

    # Link engine libraries
    target_link_libraries(${TARGET_NAME} PUBLIC ${ENGINE_PLATFORM_LIBS} ${ENGINE_LIB} ${ENGINE_DEPENDENCIES})

    # Setup HeaderForge dependency
    setup_header_forge()
    add_dependencies(${TARGET_NAME} HeaderForge)

    # --------------------------
    # Shader Embedding
    # --------------------------

    # Copy the shaders
    set(ENGINE_TEMP_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/Temp")
    file(MAKE_DIRECTORY "${ENGINE_TEMP_LOCATION}/Engine")
    file(COPY "${ENGINE_LOCATION}/Engine/Shaders" DESTINATION "${ENGINE_TEMP_LOCATION}/Engine")

    # Embed the shaders
    file(GLOB_RECURSE SHADERS "${ENGINE_TEMP_LOCATION}/Engine/Shaders/*.glsl")
    cmrc_add_resource_library(engine_shaders_${TARGET_NAME}
        NAMESPACE engine_resources
        WHENCE "${ENGINE_TEMP_LOCATION}/Engine/Shaders"
        PREFIX "Engine/Shaders"
        ${SHADERS}
    )
    target_link_libraries(${TARGET_NAME} PRIVATE engine_shaders_${TARGET_NAME})

    # Remove the copied shaders
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${ENGINE_TEMP_LOCATION}"
    )

    # Move executable to parent directory
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rename $<TARGET_FILE:${TARGET_NAME}> ${CMAKE_CURRENT_BINARY_DIR}/../$<TARGET_FILE_NAME:${TARGET_NAME}>
    )

    # --------------------------
    # Macros
    # --------------------------

    target_compile_definitions(${TARGET_NAME} PUBLIC
        $<$<CONFIG:Debug>:GAME_DEBUG=1>
        MULTI_THREAD=1
        ENGINE_EDITOR=$<BOOL:$<STREQUAL:${PROJECT_BUILD_TYPE},shared>>
    )
endfunction()

# Export variables for use in parent CMakeLists
set(ENGINE_EDITOR_FILES ${EDITOR_FILES})
set(ENGINE_INCLUDES ${ENGINE_LOCATION}/Engine ${EDITOR_INCLUDE})