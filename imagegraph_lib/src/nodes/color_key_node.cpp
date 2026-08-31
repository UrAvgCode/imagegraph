#include <imagegraph/nodes/color_key_node.h>

#include <imagegraph/widgets/image_preview.h>

#include <shader/color_key.h>

namespace imagegraph::nodes {
    ColorKeyNode::ColorKeyNode() : _key_color(0.0, 1.0, 0.0), _tolerance(0.25f) {
        _input_pins.emplace_back(graph::Pin::Type::Texture, this);
        _output_pins.emplace_back(graph::Pin::Type::Mask, this);

        _compute_program.load(shader::color_key);
    }

    void ColorKeyNode::draw() {
        begin_node("Color Key");

        constexpr float total_width = 200.0f;
        constexpr float color_label_width = 37.0f;
        constexpr float tolerance_label_width = 70.0f;
        constexpr float color_input_width = total_width - color_label_width;
        constexpr float tolerance_input_width = total_width - tolerance_label_width;

        constexpr float max_tolerance = std::sqrt(3.0f);

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Color");

        ImGui::SameLine(color_label_width);
        ImGui::SetNextItemWidth(color_input_width);
        if (ImGui::ColorEdit3("##key_color", &_key_color.r, ImGuiColorEditFlags_Float)) {
            modified();
        }

        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Tolerance");

        ImGui::SameLine(tolerance_label_width);
        ImGui::SetNextItemWidth(tolerance_input_width);
        if (ImGui::SliderFloat("##tolerance", &_tolerance, 0.0f, max_tolerance, "%.3f")) {
            modified();
        }

        widgets::image_preview(_mask);

        end_node();
    }

    void ColorKeyNode::evaluate() {
        if (!_modified) {
            return;
        }
        _modified = false;

        const auto input_image = _input_pins[0].texture();
        if (!input_image || input_image->id() == 0) {
            _mask = compute::Mask();
            _output_pins[0].set_mask(nullptr);
            return;
        }

        const auto width = input_image->width();
        const auto height = input_image->height();

        _mask.allocate(width, height);

        input_image->bind_image(0, GL_READ_ONLY);
        _mask.bind_image(1, GL_WRITE_ONLY);

        _compute_program.set_uniform_vec3(0, _key_color);
        _compute_program.set_uniform_float(1, _tolerance);
        _compute_program.dispatch(width, height);

        _output_pins[0].set_mask(&_mask);
    }

    nlohmann::json ColorKeyNode::serialize() const {
        return {{"key_color_r", _key_color.r},
                {"key_color_g", _key_color.g},
                {"key_color_b", _key_color.b},
                {"tolerance", _tolerance}};
    }

    void ColorKeyNode::deserialize(const nlohmann::json& json) {
        if (json.contains("key_color_r") && json["key_color_r"].is_number()) {
            _key_color.r = json["key_color_r"].get<float>();
        }
        if (json.contains("key_color_g") && json["key_color_g"].is_number()) {
            _key_color.g = json["key_color_g"].get<float>();
        }
        if (json.contains("key_color_b") && json["key_color_b"].is_number()) {
            _key_color.b = json["key_color_b"].get<float>();
        }

        if (json.contains("tolerance") && json["tolerance"].is_number()) {
            _tolerance = json["tolerance"].get<float>();
        }
        modified();
    }
} // namespace imagegraph::nodes
