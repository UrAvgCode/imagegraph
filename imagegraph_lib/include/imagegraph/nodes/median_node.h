#pragma once

#include <imagegraph/compute/compute_program.h>
#include <imagegraph/compute/texture.h>
#include <imagegraph/graph/node.h>

#include <array>

namespace imagegraph::nodes {
    class MedianNode final : public graph::Node {
    public:
        MedianNode();

        void draw() override;
        void evaluate() override;

        nlohmann::json serialize() const override;
        void deserialize(const nlohmann::json&) override;

    private:
        int _radius;

        compute::Texture _texture;
        std::array<compute::ComputeProgram, 6> _compute_programs;
    };
} // namespace imagegraph::nodes
