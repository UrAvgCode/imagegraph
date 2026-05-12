#pragma once

#include <graph/node.h>
#include <image/texture.h>
#include <process/compute_program.h>

namespace imagegraph::nodes {
    class BrightnessContrastNode final : public graph::Node {
    public:
        BrightnessContrastNode();

        void draw() override;
        void evaluate() override;

    private:
        float _brightness;
        float _contrast;

        image::Texture _texture;
        process::ComputeProgram _compute_program;
    };
} // namespace imagegraph::nodes
