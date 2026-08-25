#pragma once

#include <imagegraph/compute/compute_program.h>
#include <imagegraph/compute/mask.h>
#include <imagegraph/graph/node.h>

namespace imagegraph::nodes {
    class MaskGrowShrinkNode final : public graph::Node {
    public:
        MaskGrowShrinkNode();

        void draw() override;
        void evaluate() override;

        nlohmann::json serialize() const override;
        void deserialize(const nlohmann::json&) override;

    private:
        int _amount;

        compute::Mask _mask;
        compute::ComputeProgram _compute_program;
    };
} // namespace imagegraph::nodes
