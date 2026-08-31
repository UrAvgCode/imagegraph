#include <imagegraph/nodes/color_intensity_node.h>

#include <imagegraph/widgets/image_preview.h>

#include <shader/color_intensity.h>

namespace imagegraph::nodes {
    ColorIntensityNode::ColorIntensityNode() : ColorIntensityNode(1.0f, 0.0f) {}

    ColorIntensityNode::ColorIntensityNode(const float saturation, const float vibrance) :
        _saturation(saturation), _vibrance(vibrance) {
        _input_pins.emplace_back(graph::Pin::Type::Texture, this);
        _output_pins.emplace_back(graph::Pin::Type::Texture, this);

        _compute_program.load(shader::color_intensity);
    }

    void ColorIntensityNode::draw() {
        begin_node("Color Intensity");

        constexpr float total_width = 200.0f;
        constexpr float label_width = 80.0f;
        constexpr float input_width = total_width - label_width;

        ImGui::PushItemWidth(input_width);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Saturation");
        ImGui::SameLine(label_width);
        if (ImGui::DragFloat("##saturation", &_saturation, 0.01, 0.0f, 0.0f, "%.2f")) {
            modified();
        }
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Vibrance");
        ImGui::SameLine(label_width);
        if (ImGui::DragFloat("##vibrance", &_vibrance, 0.01, 0.0f, 0.0f, "%.2f")) {
            modified();
        }
        ImGui::PopItemWidth();

        widgets::image_preview(_texture);

        end_node();
    }

    void ColorIntensityNode::evaluate() {
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

        _texture.allocate(width, height);

        _compute_program.set_uniform_float(0, _saturation);
        _compute_program.set_uniform_float(1, _vibrance);

        input_texture->bind_image(0, GL_READ_ONLY);
        _texture.bind_image(1, GL_WRITE_ONLY);

        _compute_program.dispatch(width, height);

        _output_pins[0].set_texture(&_texture);
    }

    nlohmann::json ColorIntensityNode::serialize() const {
        return {{"saturation", _saturation}, {"vibrance", _vibrance}};
    }

    void ColorIntensityNode::deserialize(const nlohmann::json& json) {
        if (json.contains("saturation") && json["saturation"].is_number()) {
            _saturation = json["saturation"].get<float>();
        }
        if (json.contains("vibrance") && json["vibrance"].is_number()) {
            _vibrance = json["vibrance"].get<float>();
        }
        modified();
    }
} // namespace imagegraph::nodes
