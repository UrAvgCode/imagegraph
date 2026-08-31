#include <imagegraph/nodes/histogram_equalization_node.h>

#include <imagegraph/widgets/image_preview.h>

#include <shader/histogram.h>
#include <shader/histogram_equalization.h>
#include <shader/lookup_table.h>

namespace imagegraph::nodes {
    HistogramEqualizationNode::HistogramEqualizationNode() {
        _input_pins.emplace_back(graph::Pin::Type::Texture, this);
        _output_pins.emplace_back(graph::Pin::Type::Texture, this);

        _histogram_program.load(shader::histogram);
        _lookup_table_program.load(shader::lookup_table);
        _histogram_equalization_program.load(shader::histogram_equalization);

        _histogram_buffer.allocate(256 * sizeof(std::uint32_t));
        _lookup_table_buffer.allocate(256 * sizeof(float));
    }

    void HistogramEqualizationNode::draw() {
        begin_node("Histogram Equalization");

        const auto texture_size = ImVec2(static_cast<float>(_texture.width()), static_cast<float>(_texture.height()));
        widgets::image_preview(_texture.id(), texture_size);

        end_node();
    }

    void HistogramEqualizationNode::evaluate() {
        if (!_modified) {
            return;
        }
        _modified = false;

        const auto input_texture = _input_pins[0].texture();
        if (!input_texture || input_texture->id() == 0) {
            _texture = compute::Texture();
            _output_pins[0].set_texture(nullptr);
            return;
        }

        const auto width = input_texture->width();
        const auto height = input_texture->height();
        if (width == 0 || height == 0) {
            _texture = compute::Texture();
            _output_pins[0].set_texture(nullptr);
            return;
        }

        _texture.allocate(width, height);

        _histogram_buffer.clear();
        _histogram_buffer.bind_base(0);
        _lookup_table_buffer.bind_base(1);

        input_texture->bind_image(0, GL_READ_ONLY);
        _texture.bind_image(1, GL_WRITE_ONLY);

        _histogram_program.dispatch(width, height);
        _lookup_table_program.dispatch(1, 1);
        _histogram_equalization_program.dispatch(width, height);

        _output_pins[0].set_texture(&_texture);
    }

    nlohmann::json HistogramEqualizationNode::serialize() const { return {}; }

    void HistogramEqualizationNode::deserialize(const nlohmann::json& json) { modified(); }
} // namespace imagegraph::nodes
