#include <benchmark/helper/input_node.h>

#include <benchmark/data/peppers.h>

#include <imagegraph/compute/transfer.h>
#include <imagegraph/image/io.h>

namespace imagegraph::benchmark {
    InputNode::InputNode() {
        _output_pins.emplace_back(graph::Pin::Type::Texture, this).set_texture(&_texture);

        _image = image::load_from_memory(peppers_jpg.data(), peppers_jpg.size());
        compute::upload_image(_image, &_texture);
    }

    void InputNode::draw() {}

    void InputNode::evaluate() {}

    nlohmann::json InputNode::serialize() const { return {}; }

    void InputNode::deserialize(const nlohmann::json&) {}

    void InputNode::set_size(const int width, const int height) {
        _image = image::load_from_memory(peppers_jpg.data(), peppers_jpg.size());
        _image.resize(width, height);
        compute::upload_image(_image, &_texture);
    }

    const image::Image& InputNode::image() { return _image; }
} // namespace imagegraph::benchmark
