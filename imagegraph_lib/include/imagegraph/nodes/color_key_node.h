#pragma once

#include <imagegraph/compute/compute_program.h>
#include <imagegraph/compute/mask.h>
#include <imagegraph/graph/node.h>

#include <glm/vec3.hpp>

namespace imagegraph::nodes {
    class ColorKeyNode final : public graph::Node {
    public:
        ColorKeyNode();

        void draw() override;
        void evaluate() override;

        nlohmann::json serialize() const override;
        void deserialize(const nlohmann::json&) override;

    private:
        glm::vec3 _key_color;
        float _tolerance;

        compute::Mask _mask;
        compute::ComputeProgram _compute_program;
    };
} // namespace imagegraph::nodes
