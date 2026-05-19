#include <nodes/blend_node.h>

#include <compute/mask.h>
#include <shader/blend.h>
#include <widgets/image_preview.h>

namespace imagegraph::nodes {
    BlendNode::BlendNode() {
        _input_pins.emplace_back(graph::Pin::Type::Texture, this, "base");
        _input_pins.emplace_back(graph::Pin::Type::Texture, this, "blend");
        _input_pins.emplace_back(graph::Pin::Type::Mask, this, "mask");
        _output_pins.emplace_back(graph::Pin::Type::Texture, this);

        _compute_program.load(shader::blend);
    }

    void BlendNode::draw() {
        ax::NodeEditor::BeginNode(_id);
        ImGui::PushID(_id.AsPointer());

        ImGui::Text("Blend");

        ImGui::BeginGroup();
        for (const auto& pin: _input_pins) {
            pin.draw();
        }
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        const auto texture_size = ImVec2(static_cast<float>(_texture.width()), static_cast<float>(_texture.height()));
        widgets::image_preview(_texture.id(), texture_size);
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

    void BlendNode::evaluate() {
        if (!_modified) {
            return;
        }
        _modified = false;

        const auto base_texture = _input_pins[0].texture();
        const auto blend_texture = _input_pins[1].texture();
        if (!base_texture || base_texture->id() == 0 || !blend_texture || blend_texture->id() == 0) {
            _texture = compute::Texture();
            _output_pins[0].set_texture(nullptr);
            return;
        }

        const auto width = base_texture->width();
        const auto height = base_texture->height();
        if (width == 0 || height == 0 || width != blend_texture->width() || height != blend_texture->height()) {
            return;
        }

        _texture.allocate(width, height);

        _compute_program.bind();

        const auto mask = _input_pins[2].mask();
        if (mask && mask->id() != 0) {
            mask->bind(2);
            _compute_program.set_uniform_int("u_use_mask", 1);
        } else {
            compute::Mask::unbind(2);
            _compute_program.set_uniform_int("u_use_mask", 0);
        }

        base_texture->bind_image(0, GL_READ_ONLY);
        blend_texture->bind_image(1, GL_READ_ONLY);
        _texture.bind_image(3, GL_WRITE_ONLY);

        _compute_program.dispatch(width, height);

        compute::ComputeProgram::unbind();
        compute::Texture::unbind_image(0);
        compute::Texture::unbind_image(1);
        compute::Texture::unbind_image(3);
        compute::Mask::unbind(2);

        _output_pins[0].set_texture(&_texture);
    }

    nlohmann::json BlendNode::serialize() const { return {}; }

    void BlendNode::deserialize(const nlohmann::json& json) {}
} // namespace imagegraph::nodes
