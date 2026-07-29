#include <imagegraph/nodes/depth_node.h>

#include <imagegraph/compute/transfer.h>
#include <imagegraph/inference/environment.h>
#include <imagegraph/widgets/image_preview.h>
#include <imagegraph/widgets/indeterminate_progress_bar.h>

#include <algorithm>
#include <utility>

namespace imagegraph::nodes {
    DepthNode::DepthNode() : _output_size({518, 518}), _processing(false) {
        _input_pins.emplace_back(graph::Pin::Type::Texture, this);
        _output_pins.emplace_back(graph::Pin::Type::Mask, this);
    }

    void DepthNode::draw() {
        ax::NodeEditor::BeginNode(_id);
        ImGui::PushID(_id.AsPointer());
        {
            ImGui::Text("Depth");

            ImGui::BeginGroup();
            for (const auto& pin: _input_pins) {
                pin.draw();
            }
            ImGui::EndGroup();

            ImGui::SameLine();

            ImGui::BeginGroup();
            {
                constexpr float total_width = 200.0f;
                constexpr float label_width = 35.0f;
                constexpr float input_width = total_width - label_width;

                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("Size");

                ImGui::SameLine(label_width);
                ImGui::SetNextItemWidth(input_width);
                if (ImGui::InputInt2("##output_size", _output_size.data())) {
                    _output_size[0] = std::clamp((_output_size[0] / 14) * 14, 14, 2072);
                    _output_size[1] = std::clamp((_output_size[1] / 14) * 14, 14, 2072);
                }
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    modified();
                }

                widgets::indeterminate_progress_bar(_processing);

                const auto mask_size = ImVec2(static_cast<float>(_mask.width()), static_cast<float>(_mask.height()));
                widgets::image_preview(_mask.id(), mask_size);
            }
            ImGui::EndGroup();

            ImGui::SameLine();

            ImGui::BeginGroup();
            for (auto& pin: _output_pins) {
                pin.draw();
            }
            ImGui::EndGroup();
        }
        ImGui::PopID();
        ax::NodeEditor::EndNode();
    }

    void DepthNode::evaluate() {
        if (_processing && _future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            const auto result = _future.get();

            if (result.width() && result.height()) {
                compute::upload_mask(result, &_mask);
                _output_pins[0].set_mask(&_mask);
            }

            _processing = false;
        }

        if (!_modified || _processing) {
            return;
        }
        _modified = false;

        const auto input_texture = _input_pins[0].texture();
        if (!input_texture || input_texture->id() == 0) {
            _mask = compute::Mask();
            _output_pins[0].set_mask(nullptr);
            return;
        }

        auto input_data = compute::download_texture(input_texture);
        _future = std::async(std::launch::async, &inference::DepthModel::run, inference::get_depth_model(),
                             std::move(input_data), _output_size);

        _processing = true;
    }

    nlohmann::json DepthNode::serialize() const {
        return {{"output_width", _output_size[0]}, {"output_height", _output_size[1]}};
    }

    void DepthNode::deserialize(const nlohmann::json& json) {
        if (json.contains("output_width") && json["output_width"].is_number()) {
            _output_size[0] = json["output_width"].get<int>();
        }
        if (json.contains("output_height") && json["output_height"].is_number()) {
            _output_size[1] = json["output_height"].get<int>();
        }
        modified();
    }
} // namespace imagegraph::nodes
