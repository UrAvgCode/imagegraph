add_subdirectory(glfw)
add_subdirectory(glad)
add_subdirectory(glm)

add_library(imgui STATIC
        imgui/imgui.cpp
        imgui/imgui_draw.cpp
        imgui/imgui_widgets.cpp
        imgui/imgui_tables.cpp
        imgui/misc/cpp/imgui_stdlib.cpp
        imgui/backends/imgui_impl_glfw.cpp
        imgui/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC imgui imgui/backends)
target_link_libraries(imgui PRIVATE glfw)
