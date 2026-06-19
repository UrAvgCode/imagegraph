#include <imagegraph/nodes/color_intensity_node.h>

#include <imagegraph/widgets/image_preview.h>

#include <shader/color_intensity.h>

namespace imagegraph::nodes {
    ColorIntensityNode::ColorIntensityNode() : _saturation(1.0f), _vibrance(0.0f) {
        _input_pins.emplace_back(graph::Pin::Type::Texture, this);
        _output_pins.emplace_back(graph::Pin::Type::Texture, this);

        _compute_program.load(shader::color_intensity);
    }

    void ColorIntensityNode::draw() {
        ax::NodeEditor::BeginNode(_id);
        ImGui::PushID(_id.AsPointer());

        ImGui::Text("Color Intensity");

        ImGui::BeginGroup();
        for (const auto& pin: _input_pins) {
            pin.draw();
        }
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        {
            constexpr float total_width = 200.0f;
            constexpr float label_width = 80.0f;
            constexpr float input_width = total_width - label_width;

            ImGui::PushItemWidth(input_width);
            ImGui::TextUnformatted("Saturation");
            ImGui::SameLine(label_width);
            if (ImGui::DragFloat("##saturation", &_saturation, 0.01, 0.0f, 0.0f, "%.2f")) {
                modified();
            }
            ImGui::TextUnformatted("Vibrance");
            ImGui::SameLine(label_width);
            if (ImGui::DragFloat("##vibrance", &_vibrance, 0.01, 0.0f, 0.0f, "%.2f")) {
                modified();
            }
            ImGui::PopItemWidth();

            const auto texture_size =
                    ImVec2(static_cast<float>(_texture.width()), static_cast<float>(_texture.height()));
            widgets::image_preview(_texture.id(), texture_size);
        }
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        for (auto& pin: _output_pins) {
            pin.draw();
        }
        ImGui::EndGroup();

        ImGui::PopID();
        ax::NodeEditor::EndNode();
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
        if (width == 0 || height == 0) {
            return;
        }

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
