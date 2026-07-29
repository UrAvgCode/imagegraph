#include <imagegraph/nodes/bokeh_node.h>

#include <imagegraph/widgets/image_preview.h>

#include <shader/bokeh.h>

#include <algorithm>

namespace imagegraph::nodes {
    BokehNode::BokehNode() : _focus(1.0f), _radius(20.0f) {
        _input_pins.emplace_back(graph::Pin::Type::Texture, this, "image");
        _input_pins.emplace_back(graph::Pin::Type::Mask, this, "depth");
        _output_pins.emplace_back(graph::Pin::Type::Texture, this);

        _compute_program.load(shader::bokeh);
    }

    void BokehNode::draw() {
        ax::NodeEditor::BeginNode(_id);
        ImGui::PushID(_id.AsPointer());

        ImGui::Text("Bokeh");

        ImGui::BeginGroup();
        for (const auto& pin: _input_pins) {
            pin.draw();
        }
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        {
            constexpr float total_width = 200.0f;
            constexpr float label_width = 50.0f;
            constexpr float input_width = total_width - label_width;

            ImGui::PushItemWidth(input_width);
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Focus");
            ImGui::SameLine(label_width);
            if (ImGui::DragFloat("##focus", &_focus, 0.01, 0.0f, 0.0f, "%.2f")) {
                _focus = std::clamp(_focus, 0.0f, 1.0f);
                modified();
            }
            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Radius");
            ImGui::SameLine(label_width);
            if (ImGui::DragFloat("##radius", &_radius, 0.01, 0.0f, 0.0f, "%.2f")) {
                _radius = std::clamp(_radius, 1.0f, 30.0f);
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

    void BokehNode::evaluate() {
        if (!_modified) {
            return;
        }
        _modified = false;

        const auto input_texture = _input_pins[0].texture();
        const auto mask = _input_pins[1].mask();
        if (!input_texture || !*input_texture || !mask || !*mask) {
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

        input_texture->bind_image(0, GL_READ_ONLY);
        _texture.bind_image(1, GL_WRITE_ONLY);
        mask->bind(2);

        _compute_program.set_uniform_float(0, _focus);
        _compute_program.set_uniform_float(1, _radius);
        _compute_program.dispatch(width, height);

        _output_pins[0].set_texture(&_texture);
    }

    nlohmann::json BokehNode::serialize() const { return {{"focus", _focus}, {"radius", _radius}}; }

    void BokehNode::deserialize(const nlohmann::json& json) {
        if (json.contains("focus") && json["focus"].is_number()) {
            _focus = json["focus"].get<float>();
        }
        if (json.contains("radius") && json["radius"].is_number()) {
            _radius = json["radius"].get<float>();
        }
        modified();
    }
} // namespace imagegraph::nodes
