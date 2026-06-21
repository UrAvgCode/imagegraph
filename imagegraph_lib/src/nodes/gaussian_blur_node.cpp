#include <imagegraph/nodes/gaussian_blur_node.h>

#include <imagegraph/widgets/image_preview.h>

#include <shader/gaussian_blur.h>

namespace imagegraph::nodes {
    GaussianBlurNode::GaussianBlurNode() : GaussianBlurNode(3, 3) {}

    GaussianBlurNode::GaussianBlurNode(const int size_x, const int size_y) : _blur_size(size_x, size_y) {
        _input_pins.emplace_back(graph::Pin::Type::Texture, this);
        _output_pins.emplace_back(graph::Pin::Type::Texture, this);

        _compute_program.load(shader::gaussian_blur);
    }

    void GaussianBlurNode::draw() {
        ax::NodeEditor::BeginNode(_id);
        ImGui::PushID(_id.AsPointer());

        ImGui::Text("Gaussian Blur");

        ImGui::BeginGroup();
        for (const auto& pin: _input_pins) {
            pin.draw();
        }
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        {
            constexpr float total_width = 200.0f;
            constexpr float label_width = 40.0f;
            constexpr float input_width = total_width - label_width;

            ImGui::PushItemWidth(input_width);
            ImGui::TextUnformatted("Size");
            ImGui::SameLine(label_width);
            if (ImGui::DragInt2("##size", &_blur_size.x)) {
                _blur_size = glm::clamp(_blur_size, 1, 150);
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

    void GaussianBlurNode::evaluate() {
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
        _temp_texture.allocate(width, height);

        input_texture->bind_image(0, GL_READ_ONLY);
        _temp_texture.bind_image(1, GL_WRITE_ONLY);

        _compute_program.set_uniform_ivec2(0, _blur_size);
        _compute_program.set_uniform_int(1, false);
        _compute_program.dispatch(width, height);

        _temp_texture.bind_image(0, GL_READ_ONLY);
        _texture.bind_image(1, GL_WRITE_ONLY);

        _compute_program.set_uniform_int(1, true);
        _compute_program.dispatch(width, height);

        _output_pins[0].set_texture(&_texture);
    }

    nlohmann::json GaussianBlurNode::serialize() const { return {{"size_x", _blur_size.x}, {"size_y", _blur_size.y}}; }

    void GaussianBlurNode::deserialize(const nlohmann::json& json) {
        if (json.contains("size_x") && json["size_x"].is_number()) {
            _blur_size.x = json["size_x"].get<int>();
        }
        if (json.contains("size_y") && json["size_y"].is_number()) {
            _blur_size.y = json["size_y"].get<int>();
        }
        modified();
    }
} // namespace imagegraph::nodes
