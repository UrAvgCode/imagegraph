#include <nodes/brightness_contrast_node.h>

#include <shader/brightness_contrast.h>
#include <widgets/image_preview.h>

namespace imagegraph::nodes {
    BrightnessContrastNode::BrightnessContrastNode() : _brightness(0.0f), _contrast(1.0f) {
        _input_pins.emplace_back(graph::Pin::Type::Texture, this);
        _output_pins.emplace_back(graph::Pin::Type::Texture, this);

        _compute_program.load(shader::brightness_contrast);
    }

    void BrightnessContrastNode::draw() {
        ax::NodeEditor::BeginNode(_id);
        ImGui::PushID(_id.AsPointer());

        ImGui::Text("Brightness/Contrast");

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
            ImGui::TextUnformatted("Brightness");
            ImGui::SameLine(label_width);
            if (ImGui::DragFloat("##brightness", &_brightness, 0.01, 0.0f, 0.0f, "%.2f")) {
                modified();
            }
            ImGui::TextUnformatted("Contrast");
            ImGui::SameLine(label_width);
            if (ImGui::DragFloat("##contrast", &_contrast, 0.01, 0.0f, 0.0f, "%.2f")) {
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

    void BrightnessContrastNode::evaluate() {
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

        _compute_program.set_uniform_float(0, _brightness);
        _compute_program.set_uniform_float(1, _contrast);

        input_texture->bind_image(0, GL_READ_ONLY);
        _texture.bind_image(1, GL_WRITE_ONLY);

        _compute_program.dispatch(width, height);

        _output_pins[0].set_texture(&_texture);
    }

    nlohmann::json BrightnessContrastNode::serialize() const {
        return {{"brightness", _brightness}, {"contrast", _contrast}};
    }

    void BrightnessContrastNode::deserialize(const nlohmann::json& json) {
        if (json.contains("brightness") && json["brightness"].is_number()) {
            _brightness = json["brightness"].get<float>();
        }
        if (json.contains("contrast") && json["contrast"].is_number()) {
            _contrast = json["contrast"].get<float>();
        }
        modified();
    }
} // namespace imagegraph::nodes
