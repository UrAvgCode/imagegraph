#include <imagegraph/nodes/median_node.h>

#include <imagegraph/widgets/image_preview.h>

#include <shader/median.h>

#include <format>

namespace imagegraph::nodes {
    MedianNode::MedianNode() : _radius(2) {
        _input_pins.emplace_back(graph::Pin::Type::Texture, this);
        _output_pins.emplace_back(graph::Pin::Type::Texture, this);

        for (std::size_t i = 0; i < _compute_programs.size(); ++i) {
            const auto median_source = std::format("#version 460 core\n#define RADIUS {}\n{}", i, shader::median);
            _compute_programs[i].load(median_source.c_str());
        }
    }

    void MedianNode::draw() {
        ax::NodeEditor::BeginNode(_id);
        ImGui::PushID(_id.AsPointer());

        ImGui::Text("Median");

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

            ImGui::AlignTextToFramePadding();
            ImGui::TextUnformatted("Radius");

            ImGui::SameLine(label_width);
            ImGui::SetNextItemWidth(input_width);
            ImGui::SliderInt("##radius", &_radius, 0, static_cast<int>(_compute_programs.size() - 1), "%d px");
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                modified();
            }

            const auto texture_size =
                    ImVec2(static_cast<float>(_texture.width()), static_cast<float>(_texture.height()));
            widgets::image_preview(_texture.id(), texture_size);
        }
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        for (const auto& pin: _output_pins) {
            pin.draw();
        }
        ImGui::EndGroup();

        ImGui::PopID();
        ax::NodeEditor::EndNode();
    }

    void MedianNode::evaluate() {
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
            _texture = compute::Texture();
            _output_pins[0].set_texture(nullptr);
            return;
        }

        _texture.allocate(width, height);

        input_texture->bind_image(0, GL_READ_ONLY);
        _texture.bind_image(1, GL_WRITE_ONLY);

        _compute_programs[_radius].dispatch(width, height);

        _output_pins[0].set_texture(&_texture);
    }

    nlohmann::json MedianNode::serialize() const { return {{"radius", _radius}}; }

    void MedianNode::deserialize(const nlohmann::json& json) {
        if (json.contains("radius") && json["radius"].is_number()) {
            _radius = json["radius"].get<int>();
        }
        modified();
    }
} // namespace imagegraph::nodes
