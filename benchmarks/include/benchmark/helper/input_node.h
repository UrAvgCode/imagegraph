#pragma once

#include <imagegraph/compute/texture.h>
#include <imagegraph/graph/node.h>
#include <imagegraph/image/image.h>

namespace imagegraph::benchmark {
    class InputNode final : public graph::Node {
    public:
        explicit InputNode();

        void draw() override;
        void evaluate() override;

        nlohmann::json serialize() const override;
        void deserialize(const nlohmann::json&) override;

        void set_size(int, int);
        const image::Image& image();

    private:
        image::Image _image;
        compute::Texture _texture;
    };
} // namespace imagegraph::benchmark
