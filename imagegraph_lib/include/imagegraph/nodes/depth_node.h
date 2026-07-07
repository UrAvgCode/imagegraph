#pragma once

#include <imagegraph/compute/mask.h>
#include <imagegraph/graph/node.h>
#include <imagegraph/image/image.h>
#include <imagegraph/inference/depth_model.h>

#include <array>
#include <future>

namespace imagegraph::nodes {
    class DepthNode final : public graph::Node {
    public:
        DepthNode();

        void draw() override;
        void evaluate() override;

        nlohmann::json serialize() const override;
        void deserialize(const nlohmann::json&) override;

    private:
        std::array<int, 2> _output_size;
        compute::Mask _mask;

        std::future<image::Image> _future;
        bool _processing;
    };
} // namespace imagegraph::nodes
