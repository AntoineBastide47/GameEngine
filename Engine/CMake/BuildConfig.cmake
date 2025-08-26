function(setup_build_config TARGET_NAME)
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${TARGET_NAME} PRIVATE
            -O3 -DNDEBUG -finline-functions -fomit-frame-pointer
            -ffunction-sections -fdata-sections
            -fno-fast-math -fno-strict-aliasing -frounding-math
        )
        target_link_options(${TARGET_NAME} PRIVATE -Wl,-O2)
        set_target_properties(${TARGET_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_STRIP} $<TARGET_FILE:${TARGET_NAME}>)
    elseif (CMAKE_BUILD_TYPE STREQUAL "Debug")
        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
            set(SANITIZER_FLAGS "-fsanitize=address,undefined")
            set(DEBUG_FLAGS
                -g -fno-omit-frame-pointer -fsanitize-address-use-after-scope
                -Wall -Wextra -std=c++20 -pedantic -Weffc++ -Wno-unused-parameter -Wno-unused-function
            )

            # Apply compile and link flags properly
            target_compile_options(${TARGET_NAME} PRIVATE ${SANITIZER_FLAGS} ${DEBUG_FLAGS})
            target_link_options(${TARGET_NAME} PRIVATE ${SANITIZER_FLAGS} ${DEBUG_FLAGS})

            # ASAN runtime options as a macro
            add_compile_definitions(ASAN_OPTIONS="verbosity=1:halt_on_error=1:detect_leaks=1:symbolize=1")
        endif ()
    endif ()
endfunction()