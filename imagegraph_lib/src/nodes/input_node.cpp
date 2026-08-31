#include <imagegraph/nodes/input_node.h>

#include <imagegraph/compute/transfer.h>
#include <imagegraph/image/io.h>
#include <imagegraph/platform/file_dialog.h>
#include <imagegraph/widgets/image_preview.h>

#include <misc/cpp/imgui_stdlib.h>

#include <utility>

namespace imagegraph::nodes {
    InputNode::InputNode() { _output_pins.emplace_back(graph::Pin::Type::Texture, this); }

    void InputNode::draw() {
        begin_node("Input");

        constexpr float total_width = 200.0f;
        constexpr float button_width = 30.0f;
        const float spacing = ImGui::GetStyle().ItemSpacing.x;

        ImGui::PushItemWidth(total_width - button_width - spacing);
        if (ImGui::InputText("##path", &_path, ImGuiInputTextFlags_ReadOnly)) {
            modified();
        }
        ImGui::PopItemWidth();

        ImGui::SameLine();

        if (ImGui::Button("...", ImVec2(button_width, 19.0f))) {
            auto path = platform::open_image_dialog();
            if (!path.empty()) {
                _path = std::move(path);
                modified();
            }
        }

        const auto texture_size = ImVec2(static_cast<float>(_texture.width()), static_cast<float>(_texture.height()));
        widgets::image_preview(_texture.id(), texture_size);

        end_node();
    }

    void InputNode::evaluate() {
        if (_future.valid() && _future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            const auto result = _future.get();

            if (result.width() && result.height()) {
                compute::upload_image(result, &_texture);
                _output_pins[0].set_texture(&_texture);
            }
        }

        if (!_modified || _future.valid()) {
            return;
        }
        _modified = false;

        _future = image::load_from_file_async(_path);
    }

    nlohmann::json InputNode::serialize() const { return {{"path", _path}}; }

    void InputNode::deserialize(const nlohmann::json& json) {
        if (json.contains("path") && json["path"].is_string()) {
            _path = json["path"].get<std::string>();
        }
        modified();
    }
} // namespace imagegraph::nodes
