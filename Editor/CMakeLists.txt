cmake_minimum_required(VERSION 3.29)
project(EngineEditor VERSION 0.0.1)

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
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    OUTPUT_NAME ${EXECUTABLE_NAME}
    PREFIX ""
)

target_include_directories(${EXECUTABLE_NAME} PUBLIC
    include ${CMAKE_SOURCE_DIR}/SDK/Engine
)

# ImGui source files
set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/vendor/imgui)
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC
    ${CMAKE_SOURCE_DIR}/vendor/imgui
    ${CMAKE_SOURCE_DIR}/vendor/imgui/backends
    ${CMAKE_SOURCE_DIR}/vendor/glfw/include
    ${CMAKE_SOURCE_DIR}/vendor/glew/include
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

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${EXECUTABLE_NAME} PRIVATE
        -O3 -DNDEBUG -finline-functions -fomit-frame-pointer
        -ffunction-sections -fdata-sections
        -fno-fast-math -fno-strict-aliasing -frounding-math
    )
    target_link_options(${EXECUTABLE_NAME} PRIVATE -Wl,-O2)
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    add_custom_command(TARGET ${EXECUTABLE_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${EXECUTABLE_NAME}>)
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        set(SANITIZER_FLAGS "-fsanitize=address,undefined")
        set(DEBUG_FLAGS -g -fno-omit-frame-pointer -fsanitize-address-use-after-scope)

        # Apply compile and link flags properly
        target_compile_options(${EXECUTABLE_NAME} PRIVATE ${SANITIZER_FLAGS} ${DEBUG_FLAGS})
        target_link_options(${EXECUTABLE_NAME} PRIVATE ${SANITIZER_FLAGS} ${DEBUG_FLAGS})

        # ASAN runtime options as a macro
        add_compile_definitions(ASAN_OPTIONS="verbosity=1:halt_on_error=1:detect_leaks=1:symbolize=1")
    endif ()
endif ()

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
)