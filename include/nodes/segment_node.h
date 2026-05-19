#pragma once

#include <onnxruntime/onnxruntime_cxx_api.h>

#include <compute/compute_program.h>
#include <compute/mask.h>
#include <graph/node.h>

#include <array>
#include <future>
#include <string>
#include <vector>

namespace imagegraph::nodes {
    struct DecoderInputs {
        Ort::Value image_embed;
        Ort::Value high_res_feats_0;
        Ort::Value high_res_feats_1;
        Ort::Value point_coords;
        Ort::Value point_labels;
        Ort::Value mask_input;
        Ort::Value has_mask_input;
    };

    static_assert(sizeof(DecoderInputs) == sizeof(Ort::Value) * 7);

    class SegmentNode final : public graph::Node {
    public:
        SegmentNode();

        void draw() override;
        void evaluate() override;

        nlohmann::json serialize() const override;
        void deserialize(const nlohmann::json&) override;

    private:
        std::array<float, 2> _uv;
        float _threshold;

        bool _uv_modified;
        bool _threshold_modified;

        compute::Mask _mask;
        compute::Mask _logits_mask;
        compute::ComputeProgram _compute_program;

        Ort::Env _env;
        Ort::Session _encoder_session;
        Ort::Session _decoder_session;

        std::vector<std::string> _encoder_input_name_storage;
        std::vector<std::string> _encoder_output_name_storage;

        std::vector<std::string> _decoder_input_name_storage;
        std::vector<std::string> _decoder_output_name_storage;

        std::vector<const char*> _encoder_input_names;
        std::vector<const char*> _encoder_output_names;

        std::vector<const char*> _decoder_input_names;
        std::vector<const char*> _decoder_output_names;

        std::future<DecoderInputs> _future;
        bool _processing;

        DecoderInputs _decoder_inputs;
    };
} // namespace imagegraph::nodes
