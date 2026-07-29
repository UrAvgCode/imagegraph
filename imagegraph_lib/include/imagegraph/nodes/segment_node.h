#pragma once

#include <imagegraph/compute/compute_program.h>
#include <imagegraph/compute/mask.h>
#include <imagegraph/graph/node.h>
#include <imagegraph/inference/segment_model.h>

#include <future>
#include <vector>

namespace imagegraph::nodes {
    class SegmentNode final : public graph::Node {
    public:
        SegmentNode();

        void draw() override;
        void evaluate() override;

        nlohmann::json serialize() const override;
        void deserialize(const nlohmann::json&) override;

    private:
        std::vector<inference::PointPrompt> _prompts;
        bool _prompts_modified;

        int _mask_index;
        bool _mask_index_modified;

        float _threshold;
        bool _threshold_modified;

        std::array<image::Image, 3> _masks;

        compute::Mask _mask;
        compute::Mask _logits_mask;
        compute::ComputeProgram _compute_program;

        std::future<inference::DecoderInputs> _future;
        bool _processing;

        inference::DecoderInputs _decoder_inputs;
    };
} // namespace imagegraph::nodes
