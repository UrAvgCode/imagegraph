#pragma once

#include <imagegraph/compute/compute_program.h>
#include <imagegraph/compute/texture.h>
#include <imagegraph/graph/node.h>

#include <glm/glm.hpp>

namespace imagegraph::nodes {
    class GaussianBlurNode final : public graph::Node {
    public:
        GaussianBlurNode();

        void draw() override;
        void evaluate() override;

        nlohmann::json serialize() const override;
        void deserialize(const nlohmann::json&) override;

    private:
        glm::ivec2 _blur_size;

        compute::Texture _texture;
        compute::Texture _temp_texture;
        compute::ComputeProgram _compute_program;
    };
} // namespace imagegraph::nodes
