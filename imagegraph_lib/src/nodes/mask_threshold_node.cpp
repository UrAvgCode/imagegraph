#include <imagegraph/nodes/mask_threshold_node.h>

#include <imagegraph/widgets/image_preview.h>

#include <shader/mask_threshold.h>

#include <algorithm>

namespace imagegraph::nodes {
    MaskThresholdNode::MaskThresholdNode() : _lower_threshold(0.5f), _upper_threshold(1.0f) {
        _input_pins.emplace_back(graph::Pin::Type::Mask, this);
        _output_pins.emplace_back(graph::Pin::Type::Mask, this);

        _compute_program.load(shader::mask_threshold);
    }

    void MaskThresholdNode::draw() {
        begin_node("Threshold Mask");

        constexpr float total_width = 200.0f;
        constexpr float label_width = 50.0f;
        constexpr float input_width = total_width - label_width;

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Range");

        ImGui::SameLine(label_width);
        ImGui::SetNextItemWidth(input_width);
        if (ImGui::DragFloatRange2("##threshold", &_lower_threshold, &_upper_threshold, 0.001f, 0.0f, 1.0f)) {
            modified();
        }

        widgets::image_preview(_mask);

        end_node();
    }

    void MaskThresholdNode::evaluate() {
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

        _compute_program.set_uniform_float(0, _lower_threshold);
        _compute_program.set_uniform_float(1, _upper_threshold);
        _compute_program.dispatch(width, height);

        _output_pins[0].set_mask(&_mask);
    }

    nlohmann::json MaskThresholdNode::serialize() const {
        return {{"lower_threshold", _lower_threshold}, {"upper_threshold", _upper_threshold}};
    }

    void MaskThresholdNode::deserialize(const nlohmann::json& json) {
        if (json.contains("lower_threshold") && json["lower_threshold"].is_number()) {
            _lower_threshold = json["lower_threshold"].get<float>();
        }
        if (json.contains("upper_threshold") && json["upper_threshold"].is_number()) {
            _upper_threshold = json["upper_threshold"].get<float>();
        }
        modified();
    }
} // namespace imagegraph::nodes
