#pragma once

#include <imagegraph/image/image.h>
#include <imagegraph/inference/tensor_names.h>

#include <onnxruntime_cxx_api.h>

#include <array>
#include <type_traits>

namespace imagegraph::inference {
    struct DecoderInputs {
        Ort::Value image_embed{nullptr};
        Ort::Value high_res_feats_0{nullptr};
        Ort::Value high_res_feats_1{nullptr};
        Ort::Value point_coords{nullptr};
        Ort::Value point_labels{nullptr};
        Ort::Value mask_input{nullptr};
        Ort::Value has_mask_input{nullptr};

        Ort::Value* data();
    };

    static_assert(std::is_standard_layout_v<DecoderInputs>);
    static_assert(sizeof(DecoderInputs) == sizeof(Ort::Value) * 7);

    class SegmentModel {
    public:
        explicit SegmentModel();

        DecoderInputs encode(image::Image);
        image::Image decode(DecoderInputs&, std::array<float, 2>);

    private:
        Ort::Session _encoder_session;
        Ort::Session _decoder_session;

        TensorNames _encoder_input_names;
        TensorNames _encoder_output_names;

        TensorNames _decoder_input_names;
        TensorNames _decoder_output_names;
    };
} // namespace imagegraph::inference
