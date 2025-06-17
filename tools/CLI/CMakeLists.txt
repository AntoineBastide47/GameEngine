cmake_minimum_required(VERSION 3.30)
project(CLI VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Release")

add_executable(${PROJECT_NAME}
    main.cpp
    include/OrderedMap.hpp
    include/Command.hpp
    src/commands/Help.cpp
    include/commands/Help.hpp
    src/commands/Dependencies.cpp
    include/commands/Dependencies.hpp
    src/commands/Build.cpp
    include/commands/Build.hpp
    src/commands/CreateProject.cpp
    include/commands/CreateProject.hpp
    src/commands/LinkProject.cpp
    include/commands/LinkProject.hpp
    src/commands/Version.cpp
    include/commands/Version.hpp
    include/commands/BuildProject.hpp
    src/commands/BuildProject.cpp
    include/commands/BuildProject.hpp
    src/commands/RunProject.cpp
    include/commands/RunProject.hpp
    src/commands/Rebuild.cpp
    include/commands/Rebuild.hpp
    src/Utils.cpp
    include/Utils.hpp
    src/commands/BuildTools.cpp
    include/commands/BuildTools.hpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME engine-cli)
target_include_directories(${PROJECT_NAME} PUBLIC include)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  target_compile_options(${PROJECT_NAME} PRIVATE
      -O3 -DNDEBUG -finline-functions -fomit-frame-pointer
      -ffunction-sections -fdata-sections
      -fno-fast-math -fno-strict-aliasing -frounding-math
  )
  target_link_options(${PROJECT_NAME} PRIVATE -Wl,-O2)
  set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${PROJECT_NAME}>)
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E rename
    $<TARGET_FILE:${PROJECT_NAME}>
    ${CMAKE_CURRENT_BINARY_DIR}/../../../$<TARGET_FILE_NAME:${PROJECT_NAME}>
)