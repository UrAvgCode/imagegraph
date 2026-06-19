#pragma once

#include <compute/compute_program.h>
#include <compute/texture.h>
#include <graph/node.h>

namespace imagegraph::nodes {
    class BrightnessContrastNode final : public graph::Node {
    public:
        BrightnessContrastNode();

        void draw() override;
        void evaluate() override;

        nlohmann::json serialize() const override;
        void deserialize(const nlohmann::json&) override;

    private:
        float _brightness;
        float _contrast;

        compute::Texture _texture;
        compute::ComputeProgram _compute_program;
    };
} // namespace imagegraph::nodes
