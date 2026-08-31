#define IMGUI_DEFINE_MATH_OPERATORS
#include <imagegraph/nodes/output_node.h>

#include <imagegraph/widgets/image_preview.h>

#include <misc/cpp/imgui_stdlib.h>

#include <algorithm>
#include <format>

namespace imagegraph::nodes {
    static std::uint32_t output_counter = 0;

    OutputNode::OutputNode() : _zoom(1.0f), _pan(0, 0), _fit_request(true) {
        _input_pins.emplace_back(graph::Pin::Type::Texture, this);
        _name = std::format("Output {}", ++output_counter);
    }

    void OutputNode::draw() {
        begin_node("Output");

        ImGui::PushItemWidth(200.0f);
        ImGui::InputText("##name", &_name);
        ImGui::PopItemWidth();

        const auto current_texture = texture();

        auto texture_size = ImVec2(0, 0);
        auto texture_id = static_cast<GLuint>(0);
        if (current_texture) {
            texture_size =
                    ImVec2(static_cast<float>(current_texture->width()), static_cast<float>(current_texture->height()));
            texture_id = current_texture->id();
        }

        widgets::image_preview(texture_id, texture_size);

        end_node();
    }

    void OutputNode::evaluate() {
        if (!texture()) {
            _fit_request = true;
        }
    }

    nlohmann::json OutputNode::serialize() const { return {{"name", _name}}; }

    void OutputNode::deserialize(const nlohmann::json& json) {
        if (json.contains("name") && json["name"].is_string()) {
            _name = json["name"].get<std::string>();
        }
    }

    const std::string& OutputNode::name() const { return _name; }

    const compute::Texture* OutputNode::texture() const { return _input_pins[0].texture(); }

    void OutputNode::fit_to_canvas(const ImVec2 canvas_size) {
        const auto output_texture = texture();
        const auto texture_width = static_cast<float>(output_texture->width());
        const auto texture_height = static_cast<float>(output_texture->height());
        if (texture_width <= 0.0f || texture_height <= 0.0f) {
            return;
        }

        const auto texture_size = ImVec2(texture_width, texture_height);

        const auto padded_canvas_size = canvas_size * 0.9f;
        const auto scale = padded_canvas_size / texture_size;
        _zoom = std::min(scale.x, scale.y);

        const auto scaled_size = texture_size * _zoom;
        _pan = (canvas_size - scaled_size) * 0.5f;
    }

    bool OutputNode::consume_fit_request() {
        if (!_fit_request) {
            return false;
        }
        _fit_request = false;
        return true;
    }

    void OutputNode::zoom_at(const float factor, const ImVec2 mouse_pos) {
        const float old_zoom = _zoom;
        const float new_zoom = std::clamp(_zoom * factor, 0.05f, 20.0f);
        if (old_zoom == new_zoom) {
            return;
        }

        const auto world_pos = (mouse_pos - _pan) / old_zoom;

        _zoom = new_zoom;
        _pan = mouse_pos - world_pos * _zoom;
    }

    void OutputNode::pan_by(const ImVec2 delta) { _pan += delta; }

    float OutputNode::zoom() const { return _zoom; }

    ImVec2 OutputNode::pan() const { return _pan; }
} // namespace imagegraph::nodes
