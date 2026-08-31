#include <imagegraph/nodes/mask_invert_node.h>

#include <imagegraph/widgets/image_preview.h>

#include <shader/mask_invert.h>

namespace imagegraph::nodes {
    MaskInvertNode::MaskInvertNode() {
        _input_pins.emplace_back(graph::Pin::Type::Mask, this);
        _output_pins.emplace_back(graph::Pin::Type::Mask, this);

        _compute_program.load(shader::mask_invert);
    }

    void MaskInvertNode::draw() {
        begin_node("Invert Mask");
        widgets::image_preview(_mask);
        end_node();
    }

    void MaskInvertNode::evaluate() {
        if (!_modified) {
            return;
        }
        _modified = false;

        const auto input_mask = _input_pins[0].mask();
        if (!input_mask || input_mask->id() == 0) {
            _mask = compute::Mask();
            _output_pins[0].set_mask(nullptr);
            return;
        }

        const auto width = input_mask->width();
        const auto height = input_mask->height();

        _mask.allocate(width, height);

        input_mask->bind_image(0, GL_READ_ONLY);
        _mask.bind_image(1, GL_WRITE_ONLY);

        _compute_program.dispatch(width, height);

        _output_pins[0].set_mask(&_mask);
    }

    nlohmann::json MaskInvertNode::serialize() const { return {}; }

    void MaskInvertNode::deserialize(const nlohmann::json& json) {}
} // namespace imagegraph::nodes
