#pragma once

#include <compute/texture.h>
#include <graph/node.h>
#include <image/image.h>

#include <future>
#include <string>

namespace imagegraph::nodes {
    class InputNode final : public graph::Node {
    public:
        InputNode();

        void draw() override;
        void evaluate() override;

        nlohmann::json serialize() const override;
        void deserialize(const nlohmann::json&) override;

    private:
        std::string _path;
        compute::Texture _texture;
        std::future<image::Image> _future;
    };
} // namespace imagegraph::nodes
