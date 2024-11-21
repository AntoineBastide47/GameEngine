########################################################################################################################
#                                                                                                                      #
#                                   Engine Required CMake code, Please do not modify                                   #
#                                                                                                                      #
########################################################################################################################

cmake_minimum_required(VERSION 3.29)
project({{PROJECT_NAME}})

set(CMAKE_CXX_STANDARD 20)

# Find required packages
find_package(OpenGL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)
pkg_check_modules(GLM REQUIRED glm)
include(CMakeRC.cmake)
include(FetchContent)

# Include cpptrace to be able to print stack traces
FetchContent_Declare(
    cpptrace
    GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
    GIT_TAG        9269a72c54e0faab981aa8710b65858e2eb1f59c # <HASH or TAG>
)
FetchContent_MakeAvailable(cpptrace)

# Add the main.cpp file to build the SpaceInvaders executable
add_executable(${PROJECT_NAME} main.cpp
    GameCode/{{PROJECT_NAME}}.h
    GameCode/{{PROJECT_NAME}}.cpp
)

# Depending on the OS of the user, link the libraries correctly and find the engine library
if (APPLE)
  file(GLOB ENGINE_LIB EngineFiles/[a-zA-Z0-9]*-[0-9]*.[0-9]*.[0-9]*.a)
  target_link_libraries(${PROJECT_NAME} PRIVATE
      OpenGL::GL
      /opt/homebrew/opt/glew/lib/libGLEW.a
      /opt/homebrew/opt/glfw/lib/libglfw3.a
      /opt/homebrew/opt/glm/lib/libglm.dylib
      "-framework CoreFoundation"
      "-framework IOKit"
      "-framework Cocoa"
  )
elseif (Unix)
  message(FATAL_ERROR "Linux build is not yet supported.")
elseif (WIN32)
  message(FATAL_ERROR "Windows build is not yet supported.")
else()
  message(FATAL_ERROR "Unknown operating system, please configure the library linking.")
endif()

# Make sure the engine static library is present
if (NOT ENGINE_LIB)
  message(FATAL_ERROR "The Game Engine's 2D static library was not found in the folder EngineFiles. To copy it from it's current location to the EngineFiles, please run this command in your terminal: make refresh")
endif ()

# Embed the shaders into the executable
file(GLOB_RECURSE SHADER_FILES ${CMAKE_SOURCE_DIR}/EngineFiles/Shaders/*)
cmrc_add_resource_library(shaders NAMESPACE engine_resources ${SHADER_FILES})

# Link the libraries needed to make the engine run
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${ENGINE_LIB}
    shaders
    cpptrace::cpptrace
)

# Include the directories needed to make the engine run
target_include_directories(${PROJECT_NAME} PRIVATE
    EngineFiles
    ${OPENGL_INCLUDE_DIR}
    ${GLFW_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${cpptrace_SOURCE_DIR}/include
)

########################################################################################################################
#                                                                                                                      #
#                                         Project CMake code, modify as needed                                         #
#                                                                                                                      #
########################################################################################################################

# Embed the assets into the executable
file(GLOB_RECURSE ASSET_FILES ${CMAKE_SOURCE_DIR}/Assets/*)
cmrc_add_resource_library(assets NAMESPACE resources ${ASSET_FILES})

# Link user libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
    assets
)

# Include user include directories
target_include_directories(${PROJECT_NAME} PUBLIC

)