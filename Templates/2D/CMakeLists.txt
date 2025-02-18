########################################################################################################################
#                                                                                                                      #
#                                   Engine Required CMake code, Please do not modify                                   #
#                                                                                                                      #
########################################################################################################################

cmake_minimum_required(VERSION 3.29)
project({{PROJECT_NAME}})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Debug")

# Find required packages
find_package(GLEW REQUIRED)

# Add the main.cpp file to build the SpaceInvaders executable
add_executable(${PROJECT_NAME} main.cpp
    GameCode/{{PROJECT_NAME}}.h
    GameCode/{{PROJECT_NAME}}.cpp
)

# Depending on the OS of the user, link the libraries correctly and find the engine library
if (APPLE)
  file(GLOB ENGINE_LIB EngineInclude/[a-zA-Z0-9]*-[0-9]*.[0-9]*.[0-9]*.dylib)
  target_link_libraries(${PROJECT_NAME} PRIVATE
      "-framework CoreFoundation"
      "-framework IOKit"
      "-framework Cocoa"
  )
elseif (Unix)
  message(STATUS "Linux build is not fully supported yet, there could be some errors.")
elseif (WIN32)
  message(STATUS "Window build is not fully supported yet, there could be some errors.")
endif()

# Make sure the engine static library is present
if (NOT ENGINE_LIB)
  message(FATAL_ERROR "The Game Engine's 2D shared library was not found in the folder EngineInclude. Please check the installation of the engine and check if it has a .dylib file, if not reinstall the engine.")
endif ()

# Embed the shaders into the executable
include(CMakeRC.cmake)
file(GLOB_RECURSE SHADERS ${CMAKE_SOURCE_DIR}/EngineInclude/Shaders/*)
cmrc_add_resource_library(shaders NAMESPACE engine_resources ${SHADERS})

# Link the libraries needed to make the engine run
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${ENGINE_LIB}
    shaders
)

# Include the directories needed to make the engine run
target_include_directories(${PROJECT_NAME} PRIVATE
    EngineInclude
    ${GLEW_INCLUDE_DIRS}
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
  target_compile_options(${PROJECT_NAME} PRIVATE
      -O3                     # General optimization
      -DNDEBUG                # Disable assertions
      -finline-functions      # Enable inlining
      -flto                   # Link-Time Optimization
      -fomit-frame-pointer    # Free up a register
      -ffunction-sections     # Enable section-based optimization
      -fdata-sections         # Enable data section optimization
  )
  target_link_options(${PROJECT_NAME} PRIVATE -Wl, -O2)
  set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

  # Add stripping for Release builds
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${PROJECT_NAME}>)
elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
  # Enable Address Sanitizer
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    set(SANITIZER_FLAGS "-fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZER_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SANITIZER_FLAGS}")
    set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} ${SANITIZER_FLAGS}")
  endif ()
endif()


########################################################################################################################
#                                                                                                                      #
#                                           Game CMake code, modify as needed                                          #
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