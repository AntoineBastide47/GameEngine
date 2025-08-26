function(setup_imgui SOURCE_DIR)
    # === ImGui ===
    set(IMGUI_DIR ${SOURCE_DIR}/vendor/imgui)
    set(IMGUI_SOURCES
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    )

    if (NOT TARGET imgui)
        add_library(imgui STATIC ${IMGUI_SOURCES})
        target_include_directories(imgui PUBLIC
            ${SOURCE_DIR}/vendor/imgui
            ${SOURCE_DIR}/vendor/imgui/backends
            ${SOURCE_DIR}/vendor/glfw/include
            ${SOURCE_DIR}/vendor/glew/include
        )
    endif ()
endfunction()