#pragma once

#include <imagegraph/image/image.h>
#include <imagegraph/inference/tensor_names.h>

#include <onnxruntime_cxx_api.h>

namespace imagegraph::inference {
    class DepthModel {
    public:
        explicit DepthModel();

        image::Image run(image::Image, std::array<int, 2>);

    private:
        Ort::Session _session;
        TensorNames _input_names;
        TensorNames _output_names;
    };
} // namespace imagegraph::inference
