cmake_minimum_required(VERSION 3.24)
project(imagegraph)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_WARNINGS ON)
option(BUILD_SHARED_LIBS OFF)

include(cmake/models.cmake)
include(cmake/shaders.cmake)
include(cmake/onnxruntime.cmake)

add_subdirectory(external)

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE header_files CONFIGURE_DEPENDS include/*.h)

add_executable(imagegraph ${source_files} ${header_files})
target_compile_features(imagegraph PUBLIC cxx_std_20)
target_include_directories(imagegraph PUBLIC include)

target_link_libraries(imagegraph PRIVATE
        glfw glad glm::glm
        imgui imgui_node_editor
        nfd stb nlohmann_json::nlohmann_json
        shader_headers
        onnxruntime
)
