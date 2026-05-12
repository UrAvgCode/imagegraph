#include <nodes/brightness_contrast_node.h>

#include <shader/brightness_contrast.h>
#include <widgets/image_preview.h>

namespace imagegraph::nodes {
    BrightnessContrastNode::BrightnessContrastNode() : _brightness(0.0f), _contrast(1.0f) {
        _input_pins.emplace_back(graph::PinType::Texture, this);
        _output_pins.emplace_back(graph::PinType::Texture, this);

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
            ImGui::SameLine(80.0f);
            if (ImGui::DragFloat("##brightness", &_brightness, 0.01, 0.0f, 0.0f, "%.2f")) {
                modified();
            }
            ImGui::TextUnformatted("Contrast");
            ImGui::SameLine(80.0f);
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

        const auto input_texture = std::get<image::Texture*>(_input_pins[0].get_value());
        if (!input_texture || input_texture->id() == 0) {
            _texture = image::Texture();
            _output_pins[0].set_value({});
            return;
        }

        const auto width = input_texture->width();
        const auto height = input_texture->height();
        if (width == 0 || height == 0) {
            return;
        }

        _texture.allocate(width, height);

        _compute_program.bind();
        _compute_program.set_uniform_float("u_brightness", _brightness);
        _compute_program.set_uniform_float("u_contrast", _contrast);

        input_texture->bind_image(0, GL_READ_ONLY);
        _texture.bind_image(1, GL_WRITE_ONLY);

        _compute_program.dispatch(width, height);

        process::ComputeProgram::unbind();
        image::Texture::unbind_image(0);
        image::Texture::unbind_image(1);

        _output_pins[0].set_value(&_texture);
    }
} // namespace imagegraph::nodes
