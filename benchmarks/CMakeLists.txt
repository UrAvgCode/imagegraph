include(models)

find_package(OpenMP REQUIRED)

file(GLOB_RECURSE source_files CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE header_files CONFIGURE_DEPENDS include/*.h)

add_library(benchmark_lib STATIC ${source_files} ${header_files})
target_compile_features(benchmark_lib PUBLIC cxx_std_20)
target_include_directories(benchmark_lib PUBLIC include)
target_link_libraries(benchmark_lib PUBLIC imagegraph_lib OpenMP::OpenMP_CXX)

add_executable(grayscale_benchmark executables/grayscale_main.cpp)
target_link_libraries(grayscale_benchmark PRIVATE benchmark_lib)

add_executable(blur_benchmark executables/blur_main.cpp)
target_link_libraries(blur_benchmark PRIVATE benchmark_lib)

add_executable(histogram_benchmark executables/histogram_equalization_main.cpp)
target_link_libraries(histogram_benchmark PRIVATE benchmark_lib)

add_executable(depth_model_benchmark executables/depth_model_main.cpp)
target_link_libraries(depth_model_benchmark PRIVATE benchmark_lib)

add_executable(segment_model_benchmark executables/segment_model_main.cpp)
target_link_libraries(segment_model_benchmark PRIVATE benchmark_lib)
