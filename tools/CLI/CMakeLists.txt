cmake_minimum_required(VERSION 3.30)
project(CLI VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Release")

add_executable(${PROJECT_NAME}
    main.cpp
    include/OrderedMap.hpp
    include/Command.hpp
    src/Commands/Help.cpp
    include/Commands/Help.hpp
    src/Commands/Dependencies.cpp
    include/Commands/Dependencies.hpp
    src/Commands/BuildEngine.cpp
    include/Commands/BuildEngine.hpp
    src/Commands/CreateProject.cpp
    include/Commands/CreateProject.hpp
    src/Commands/LinkProject.cpp
    include/Commands/LinkProject.hpp
    src/Commands/Version.cpp
    include/Commands/Version.hpp
    include/Commands/BuildProject.hpp
    src/Commands/BuildProject.cpp
    include/Commands/BuildProject.hpp
    src/Commands/RunProject.cpp
    include/Commands/RunProject.hpp
    src/Commands/Rebuild.cpp
    include/Commands/Rebuild.hpp
    src/Utils.cpp
    include/Utils.hpp
    src/Commands/BuildTools.cpp
    include/Commands/BuildTools.hpp
    src/Commands/BuildSdk.cpp
    include/Commands/BuildSdk.hpp
    src/Commands/Build.cpp
    include/Commands/Build.hpp
    src/Commands/BuildEditor.cpp
    include/Commands/BuildEditor.hpp
    src/Command.cpp
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