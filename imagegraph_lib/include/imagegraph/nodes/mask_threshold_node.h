#pragma once

#include <imagegraph/compute/compute_program.h>
#include <imagegraph/compute/mask.h>
#include <imagegraph/graph/node.h>

namespace imagegraph::nodes {
    class MaskThresholdNode final : public graph::Node {
    public:
        MaskThresholdNode();

        void draw() override;
        void evaluate() override;

        nlohmann::json serialize() const override;
        void deserialize(const nlohmann::json&) override;

    private:
        float _lower_threshold;
        float _upper_threshold;

        compute::Mask _mask;
        compute::ComputeProgram _compute_program;
    };
} // namespace imagegraph::nodes
