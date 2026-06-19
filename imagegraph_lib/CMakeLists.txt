include(shaders)

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE header_files CONFIGURE_DEPENDS include/*.h)

add_library(imagegraph_lib STATIC ${source_files} ${header_files})
target_compile_features(imagegraph_lib PUBLIC cxx_std_20)
target_include_directories(imagegraph_lib PUBLIC include)

target_link_libraries(imagegraph_lib
        PUBLIC
        glfw glad glm::glm
        imgui imgui_node_editor
        nfd nlohmann_json::nlohmann_json
        onnxruntime
        PRIVATE
        stb shader_headers
)
