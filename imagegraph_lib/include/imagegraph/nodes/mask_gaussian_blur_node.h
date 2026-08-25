#pragma once

#include <imagegraph/compute/compute_program.h>
#include <imagegraph/compute/mask.h>
#include <imagegraph/graph/node.h>

#include <glm/glm.hpp>

namespace imagegraph::nodes {
    class MaskGaussianBlurNode final : public graph::Node {
    public:
        MaskGaussianBlurNode();
        MaskGaussianBlurNode(int, int);

        void draw() override;
        void evaluate() override;

        nlohmann::json serialize() const override;
        void deserialize(const nlohmann::json&) override;

    private:
        glm::ivec2 _blur_size;

        compute::Mask _mask;
        compute::Mask _temp_mask;
        compute::ComputeProgram _compute_program;
    };
} // namespace imagegraph::nodes
