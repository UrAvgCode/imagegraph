set(model_dir "${CMAKE_BINARY_DIR}/models")
file(MAKE_DIRECTORY ${model_dir})

function(download_model output url)
    if (NOT EXISTS "${output}")
        message(STATUS "downloading ${output}")

        file(DOWNLOAD
                ${url}
                ${output}
                SHOW_PROGRESS
                STATUS status
        )

        list(GET status 0 status_code)

        if (NOT status_code EQUAL 0)
            message(FATAL_ERROR "failed to download ${output}")
        endif ()
    endif ()
endfunction()

download_model(
        "${model_dir}/depth_anything.onnx"
        "https://github.com/fabio-sim/Depth-Anything-ONNX/releases/download/v2.0.0/depth_anything_v2_vitb.onnx"
)
