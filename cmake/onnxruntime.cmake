include(FetchContent)

set(ORT_VERSION 1.18.0)

set(ORT_FILE onnxruntime-linux-x64-gpu-cuda12-${ORT_VERSION}.tgz)
set(ORT_URL https://github.com/microsoft/onnxruntime/releases/download/v${ORT_VERSION}/${ORT_FILE})

FetchContent_Declare(onnxruntime URL ${ORT_URL})
FetchContent_MakeAvailable(onnxruntime)

add_library(onnxruntime SHARED IMPORTED GLOBAL)
set_target_properties(onnxruntime PROPERTIES
        IMPORTED_LOCATION ${onnxruntime_SOURCE_DIR}/lib/libonnxruntime.so
        INTERFACE_INCLUDE_DIRECTORIES ${onnxruntime_SOURCE_DIR}/include
)

function(install_onnxruntime_libs)
    file(GLOB onnxruntime_libs "${onnxruntime_SOURCE_DIR}/lib/*.so*")
    install(FILES ${onnxruntime_libs} DESTINATION lib)
endfunction()
