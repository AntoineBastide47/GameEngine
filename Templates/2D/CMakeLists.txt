cmake_minimum_required(VERSION 3.29)
project({{PROJECT_NAME}})

# --------------------------
# Engine Configuration
# --------------------------

set(ENGINE_LOCATION {{ENGINE_LOCATION}})
include(${ENGINE_LOCATION}/CMake/EngineSetup.cmake)
include(${ENGINE_LOCATION}/CMake/PlatformDetection.cmake)

########################################################################################################################
#                                                                                                                      #
#                                           Game CMake code, modify as needed                                          #
#                                                                                                                      #
########################################################################################################################

# --------------------------
# Executable Definition
# --------------------------

add_executable(${PROJECT_NAME} main.cpp ${EDITOR_FILES}
    Game/include/{{PROJECT_NAME}}.hpp
    Game/src/{{PROJECT_NAME}}.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC Game/include)

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
target_link_libraries(${PROJECT_NAME} PUBLIC assets

)

# Include user include directories
target_include_directories(${PROJECT_NAME} PUBLIC

)

# --------------------------
# Apply Engine Setup
# --------------------------

setup_engine_target(${PROJECT_NAME})
setup_platform_detection(${PROJECT_NAME})