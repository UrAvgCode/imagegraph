include(models)

find_package(OpenMP REQUIRED)

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE header_files CONFIGURE_DEPENDS include/*.h)

add_executable(imagegraph_benchmarks ${source_files} ${header_files})
target_compile_features(imagegraph_benchmarks PRIVATE cxx_std_20)
target_include_directories(imagegraph_benchmarks PRIVATE include)
target_link_libraries(imagegraph_benchmarks PRIVATE imagegraph_lib OpenMP::OpenMP_CXX)
