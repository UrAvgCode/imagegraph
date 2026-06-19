include(models)

add_executable(imagegraph main.cpp)
target_link_libraries(imagegraph PRIVATE imagegraph_lib)

install(TARGETS imagegraph RUNTIME DESTINATION .)
install_onnxruntime_libs()
install_models()
