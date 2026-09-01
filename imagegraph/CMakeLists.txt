include(models)

add_executable(imagegraph main.cpp)
target_compile_features(imagegraph PRIVATE cxx_std_20)
target_link_libraries(imagegraph PRIVATE imagegraph_lib)
set_target_properties(imagegraph PROPERTIES INSTALL_RPATH "$ORIGIN/lib")

install(TARGETS imagegraph RUNTIME DESTINATION .)
install_onnxruntime_libs()
install_models()
