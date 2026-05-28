install(TARGETS imagegraph RUNTIME DESTINATION .)

file(GLOB onnxruntime_libs ${onnxruntime_SOURCE_DIR}/lib/*.so*)
install(FILES ${onnxruntime_libs} DESTINATION lib)
install(DIRECTORY ${CMAKE_BINARY_DIR}/models DESTINATION .)

set_target_properties(imagegraph PROPERTIES
        BUILD_RPATH ${onnxruntime_SOURCE_DIR}/lib
        INSTALL_RPATH $ORIGIN/lib
)
