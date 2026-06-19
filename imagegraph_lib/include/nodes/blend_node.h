#pragma once

#include <compute/compute_program.h>
#include <compute/texture.h>
#include <graph/node.h>

namespace imagegraph::nodes {
    class BlendNode final : public graph::Node {
    public:
        BlendNode();

        void draw() override;
        void evaluate() override;

        nlohmann::json serialize() const override;
        void deserialize(const nlohmann::json&) override;

    private:
        compute::Texture _texture;
        compute::ComputeProgram _compute_program;
    };
} // namespace imagegraph::nodes
