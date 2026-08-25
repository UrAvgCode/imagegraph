#pragma once

#include <imagegraph/compute/compute_program.h>
#include <imagegraph/compute/mask.h>
#include <imagegraph/graph/node.h>

namespace imagegraph::nodes {
    class MaskInvertNode final : public graph::Node {
    public:
        MaskInvertNode();

        void draw() override;
        void evaluate() override;

        nlohmann::json serialize() const override;
        void deserialize(const nlohmann::json&) override;

    private:
        compute::Mask _mask;
        compute::ComputeProgram _compute_program;
    };
} // namespace imagegraph::nodes
