cmake_minimum_required(VERSION 3.24)
project(imagegraph)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_WARNINGS ON)
option(BUILD_SHARED_LIBS OFF)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(onnxruntime)

add_subdirectory(external)

add_subdirectory(imagegraph_lib)
add_subdirectory(imagegraph)
