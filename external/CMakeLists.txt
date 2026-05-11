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

add_library(imgui_node_editor STATIC
        imgui-node-editor/imgui_node_editor.cpp
        imgui-node-editor/imgui_node_editor_api.cpp
        imgui-node-editor/imgui_canvas.cpp
        imgui-node-editor/crude_json.cpp
)
target_include_directories(imgui_node_editor PUBLIC imgui-node-editor)
target_link_libraries(imgui_node_editor PUBLIC imgui)
