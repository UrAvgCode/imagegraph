#include <imagegraph/nodes/white_balance_node.h>

#include <imagegraph/widgets/image_preview.h>

#include <shader/white_balance.h>

namespace imagegraph::nodes {
    WhiteBalanceNode::WhiteBalanceNode() : _temperature(0.0f), _tint(0.0f) {
        _input_pins.emplace_back(graph::Pin::Type::Texture, this);
        _output_pins.emplace_back(graph::Pin::Type::Texture, this);

        _compute_program.load(shader::white_balance);
    }

    void WhiteBalanceNode::draw() {
        begin_node("White Balance");

        constexpr float total_width = 200.0f;
        constexpr float label_width = 85.0f;
        constexpr float input_width = total_width - label_width;

        ImGui::PushItemWidth(input_width);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Temperature");
        ImGui::SameLine(label_width);
        if (ImGui::DragFloat("##temperature", &_temperature, 0.01, -1.0f, 1.0f, "%.2f")) {
            modified();
        }
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Tint");
        ImGui::SameLine(label_width);
        if (ImGui::DragFloat("##tint", &_tint, 0.01, -1.0f, 1.0f, "%.2f")) {
            modified();
        }
        ImGui::PopItemWidth();

        const auto texture_size = ImVec2(static_cast<float>(_texture.width()), static_cast<float>(_texture.height()));
        widgets::image_preview(_texture.id(), texture_size);

        end_node();
    }

    void WhiteBalanceNode::evaluate() {
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

        _compute_program.set_uniform_float(0, _temperature);
        _compute_program.set_uniform_float(1, _tint);

        input_texture->bind_image(0, GL_READ_ONLY);
        _texture.bind_image(1, GL_WRITE_ONLY);

        _compute_program.dispatch(width, height);

        _output_pins[0].set_texture(&_texture);
    }

    nlohmann::json WhiteBalanceNode::serialize() const { return {{"temperature", _temperature}, {"tint", _tint}}; }

    void WhiteBalanceNode::deserialize(const nlohmann::json& json) {
        if (json.contains("temperature") && json["temperature"].is_number()) {
            _temperature = json["temperature"].get<float>();
        }
        if (json.contains("tint") && json["tint"].is_number()) {
            _tint = json["tint"].get<float>();
        }
        modified();
    }
} // namespace imagegraph::nodes
