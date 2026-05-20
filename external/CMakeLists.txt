set(GLFW_LIBRARY_TYPE STATIC)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)
add_subdirectory(glfw)

add_subdirectory(glad/cmake)
glad_add_library(glad STATIC API gl:core=4.6 EXTENSIONS NONE REPRODUCIBLE QUIET)

set(GLM_BUILD_LIBRARY OFF)
set(GLM_BUILD_TESTS OFF)
set(GLM_ENABLE_CXX_20 ON)
add_subdirectory(glm)

set(NFD_BUILD_TESTS OFF)
set(NFD_INSTALL OFF)
set(NFD_PORTAL ON)
add_subdirectory(nfd)

set(JSON_BuildTests OFF)
add_subdirectory(json)

add_library(stb INTERFACE)
target_include_directories(stb INTERFACE stb)

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
