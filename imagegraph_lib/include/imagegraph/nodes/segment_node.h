#pragma once

#include <imagegraph/compute/compute_program.h>
#include <imagegraph/compute/mask.h>
#include <imagegraph/graph/node.h>
#include <imagegraph/inference/segment_model.h>

#include <array>
#include <future>

namespace imagegraph::nodes {
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

        std::future<inference::DecoderInputs> _future;
        bool _processing;

        inference::DecoderInputs _decoder_inputs;
    };
} // namespace imagegraph::nodes
