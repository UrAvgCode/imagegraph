set(model_dir "${CMAKE_BINARY_DIR}/models")
file(MAKE_DIRECTORY ${model_dir})

option(DOWNLOAD_MODELS "download onnx models" ON)

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

if (DOWNLOAD_MODELS)
    download_model(
            "${model_dir}/depth_anything.onnx"
            "https://github.com/fabio-sim/Depth-Anything-ONNX/releases/download/v2.0.0/depth_anything_v2_vitb_dynamic.onnx"
    )

    download_model(
            "${model_dir}/segment_anything_encoder.onnx"
            "https://huggingface.co/vietanhdev/segment-anything-2-onnx-models/resolve/main/sam2_hiera_base_plus.encoder.onnx?download=true"
    )

    download_model(
            "${model_dir}/segment_anything_decoder.onnx"
            "https://huggingface.co/vietanhdev/segment-anything-2-onnx-models/resolve/main/sam2_hiera_base_plus.decoder.onnx?download=true"
    )
endif()
