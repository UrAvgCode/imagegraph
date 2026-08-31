#include <imagegraph/nodes/mask_grow_shrink_node.h>

#include <imagegraph/widgets/image_preview.h>

#include <shader/mask_grow_shrink.h>

namespace imagegraph::nodes {
    MaskGrowShrinkNode::MaskGrowShrinkNode() : _amount(0) {
        _input_pins.emplace_back(graph::Pin::Type::Mask, this);
        _output_pins.emplace_back(graph::Pin::Type::Mask, this);

        _compute_program.load(shader::mask_grow_shrink);
    }

    void MaskGrowShrinkNode::draw() {
        begin_node("Grow/Shrink Mask");

        constexpr float total_width = 200.0f;
        constexpr float label_width = 50.0f;
        constexpr float input_width = total_width - label_width;

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Amount");

        ImGui::SameLine(label_width);
        ImGui::SetNextItemWidth(input_width);
        ImGui::SliderInt("##amount", &_amount, -32, 32, "%d px");
        if (ImGui::IsItemDeactivatedAfterEdit()) {
            modified();
        }

        const auto mask_size = ImVec2(static_cast<float>(_mask.width()), static_cast<float>(_mask.height()));
        widgets::image_preview(_mask.id(), mask_size);

        end_node();
    }

    void MaskGrowShrinkNode::evaluate() {
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

        _compute_program.set_uniform_int(0, _amount);
        _compute_program.dispatch(width, height);

        _output_pins[0].set_mask(&_mask);
    }

    nlohmann::json MaskGrowShrinkNode::serialize() const { return {{"amount", _amount}}; }

    void MaskGrowShrinkNode::deserialize(const nlohmann::json& json) {
        if (json.contains("amount") && json["amount"].is_number()) {
            _amount = json["amount"].get<int>();
        }
        modified();
    }
} // namespace imagegraph::nodes
