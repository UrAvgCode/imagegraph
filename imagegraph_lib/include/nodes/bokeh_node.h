#pragma once

#include <compute/compute_program.h>
#include <compute/texture.h>
#include <graph/node.h>

namespace imagegraph::nodes {
    class BokehNode final : public graph::Node {
    public:
        BokehNode();

        void draw() override;
        void evaluate() override;

        nlohmann::json serialize() const override;
        void deserialize(const nlohmann::json&) override;

    private:
        float _focus;
        float _radius;

        compute::Texture _texture;
        compute::ComputeProgram _compute_program;
    };
} // namespace imagegraph::nodes
