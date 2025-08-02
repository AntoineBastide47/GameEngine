cmake_minimum_required(VERSION 3.29)
project(EngineEditor VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_NAME "Editor")
set(CMAKE_BINARY_DIR ./build)

add_executable(${EXECUTABLE_NAME}
    src/main.cpp
    src/ImGui/ImGui.cpp
    include/ImGui/ImGui.hpp
)

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    OUTPUT_NAME ${EXECUTABLE_NAME}-${PROJECT_VERSION}
    PREFIX ""
)

target_include_directories(${EXECUTABLE_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
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
    OpenGL::GL GLEW::GLEW glfw
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
    COMMAND ${CMAKE_COMMAND} -E rename $<TARGET_FILE:${EXECUTABLE_NAME}> ${CMAKE_CURRENT_BINARY_DIR}/../../$<TARGET_FILE_NAME:${EXECUTABLE_NAME}>
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

# --------------------------
# Macros
# --------------------------

target_compile_definitions(${EXECUTABLE_NAME} PUBLIC

)