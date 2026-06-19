#pragma once

#include <imagegraph/compute/compute_program.h>
#include <imagegraph/compute/texture.h>
#include <imagegraph/graph/node.h>

namespace imagegraph::nodes {
    class ColorIntensityNode final : public graph::Node {
    public:
        ColorIntensityNode();
        ColorIntensityNode(float saturation, float vibrance);

        void draw() override;
        void evaluate() override;

        nlohmann::json serialize() const override;
        void deserialize(const nlohmann::json&) override;

    private:
        float _saturation;
        float _vibrance;

        compute::Texture _texture;
        compute::ComputeProgram _compute_program;
    };
} // namespace imagegraph::nodes
