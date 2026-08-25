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
        ax::NodeEditor::BeginNode(_id);
        ImGui::PushID(_id.AsPointer());

        ImGui::Text("Invert Mask");

        ImGui::BeginGroup();
        for (const auto& pin: _input_pins) {
            pin.draw();
        }
        ImGui::EndGroup();

        ImGui::SameLine();

        const auto mask_size = ImVec2(static_cast<float>(_mask.width()), static_cast<float>(_mask.height()));
        widgets::image_preview(_mask.id(), mask_size);

        ImGui::SameLine();

        ImGui::BeginGroup();
        for (auto& pin: _output_pins) {
            pin.draw();
        }
        ImGui::EndGroup();

        ImGui::PopID();
        ax::NodeEditor::EndNode();
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
        if (width == 0 || height == 0) {
            _mask = compute::Mask();
            _output_pins[0].set_mask(nullptr);
            return;
        }

        _mask.allocate(width, height);

        input_mask->bind_image(0, GL_READ_ONLY);
        _mask.bind_image(1, GL_WRITE_ONLY);

        _compute_program.dispatch(width, height);

        _output_pins[0].set_mask(&_mask);
    }

    nlohmann::json MaskInvertNode::serialize() const { return {}; }

    void MaskInvertNode::deserialize(const nlohmann::json& json) {}
} // namespace imagegraph::nodes
