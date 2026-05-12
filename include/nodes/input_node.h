#pragma once

#include <graph/node.h>
#include <image/image.h>
#include <image/texture.h>

#include <future>
#include <string>

namespace imagegraph::nodes {
    class InputNode final : public graph::Node {
    public:
        InputNode();

        void draw() override;
        void evaluate() override;

    private:
        std::string _path;
        image::Texture _texture;
        std::future<image::Image> _future;
    };
} // namespace imagegraph::nodes
