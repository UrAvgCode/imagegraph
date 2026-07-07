#define IMGUI_DEFINE_MATH_OPERATORS

#include <imagegraph/nodes/segment_node.h>

#include <imagegraph/compute/transfer.h>
#include <imagegraph/image/image.h>
#include <imagegraph/image/preprocess.h>
#include <imagegraph/widgets/image_preview.h>
#include <imagegraph/widgets/indeterminate_progress_bar.h>

#include <shader/segmentation_threshold.h>

#include <algorithm>
#include <cstdio>
#include <utility>

namespace {
    auto segment_model = imagegraph::inference::SegmentModel(imagegraph::inference::Device::Cuda);
}

namespace imagegraph::nodes {
    SegmentNode::SegmentNode() :
        _uv({0.5f, 0.5f}), _threshold(0.0f), _uv_modified(false), _threshold_modified(false), _processing(false) {
        _input_pins.emplace_back(graph::Pin::Type::Texture, this);
        _output_pins.emplace_back(graph::Pin::Type::Mask, this);
        _compute_program.load(shader::segmentation_threshold);
    }

    void SegmentNode::draw() {
        ax::NodeEditor::BeginNode(_id);
        ImGui::PushID(_id.AsPointer());
        {
            ImGui::Text("Segment");

            ImGui::BeginGroup();
            for (const auto& pin: _input_pins) {
                pin.draw();
            }
            ImGui::EndGroup();

            ImGui::SameLine();

            ImGui::BeginGroup();
            {
                constexpr float total_width = 200.0f;
                constexpr float label_width = 70.0f;
                constexpr float input_width = total_width - label_width;

                ImGui::PushItemWidth(input_width);
                ImGui::TextUnformatted("UV");
                ImGui::SameLine(label_width);
                if (ImGui::InputFloat2("##uv", _uv.data())) {
                    _uv[0] = std::clamp(_uv[0], 0.0f, 1.0f);
                    _uv[1] = std::clamp(_uv[1], 0.0f, 1.0f);
                    _uv_modified = true;
                }
                ImGui::TextUnformatted("Threshold");
                ImGui::SameLine(label_width);
                if (ImGui::DragFloat("##threshold", &_threshold, 0.01, 0.0f, 0.0f, "%.2f")) {
                    _threshold_modified = true;
                }
                ImGui::PopItemWidth();

                widgets::indeterminate_progress_bar(_processing);

                constexpr auto image_size = ImVec2(100, 100);
                const auto input_texture = _input_pins[0].texture();

                if (input_texture && input_texture->id() != 0) {
                    ImGui::Image(input_texture->id(), image_size);
                    const auto image_min = ImGui::GetItemRectMin();

                    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        const auto mouse_pos = ImGui::GetMousePos();
                        const auto local_pos = mouse_pos - image_min;

                        const auto uv = local_pos / image_size;
                        _uv = {uv.x, uv.y};
                        _uv_modified = true;
                    }

                    const auto draw_list = ImGui::GetWindowDrawList();
                    const float px = image_min.x + _uv[0] * image_size.x;
                    const float py = image_min.y + _uv[1] * image_size.y;
                    draw_list->AddCircleFilled(ImVec2(px, py), 2.0f, IM_COL32(255, 0, 0, 255));
                } else {
                    widgets::image_preview(nullptr, {0.0f, 0.0f}, image_size);
                }

                ImGui::SameLine();

                const auto mask_size = ImVec2(static_cast<float>(_mask.width()), static_cast<float>(_mask.height()));
                widgets::image_preview(_mask.id(), mask_size, ImVec2(100, 100));
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

    void SegmentNode::evaluate() {
        if (_processing && _future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            _processing = false;
            _decoder_inputs = _future.get();
            _uv_modified = true;
        }

        if (_processing) {
            return;
        }

        const auto input_texture = _input_pins[0].texture();
        if (!input_texture || input_texture->id() == 0) {
            _decoder_inputs = {};
            _mask = compute::Mask();
            _output_pins[0].set_mask(nullptr);
            return;
        }

        if (_modified) {
            _modified = false;

            auto input_data = compute::download_texture(input_texture);
            _future = std::async(std::launch::async, &inference::SegmentModel::encode, &segment_model,
                                 std::move(input_data));

            _processing = true;
            return;
        }

        if (_uv_modified && _decoder_inputs.image_embed != nullptr) {
            _uv_modified = false;

            const auto result = segment_model.decode(_decoder_inputs, _uv);

            compute::upload_mask(result, &_logits_mask);
            _threshold_modified = true;
        }

        if (_threshold_modified && _logits_mask.id() != 0) {
            _threshold_modified = false;

            _mask.allocate(input_texture->width(), input_texture->height());

            _compute_program.set_uniform_float(0, _threshold);

            _logits_mask.bind(0);
            _mask.bind_image(1, GL_WRITE_ONLY);

            _compute_program.dispatch(_mask.width(), _mask.height());

            _output_pins[0].set_mask(&_mask);
        }
    }

    nlohmann::json SegmentNode::serialize() const {
        return {{"uv_x", _uv[0]}, {"uv_y", _uv[1]}, {"threshold", _threshold}};
    }

    void SegmentNode::deserialize(const nlohmann::json& json) {
        if (json.contains("uv_x") && json["uv_x"].is_number()) {
            _uv[0] = json["uv_x"].get<float>();
        }
        if (json.contains("uv_y") && json["uv_y"].is_number()) {
            _uv[1] = json["uv_y"].get<float>();
        }
        if (json.contains("threshold") && json["threshold"].is_number()) {
            _threshold = json["threshold"].get<float>();
        }
        modified();
    }
} // namespace imagegraph::nodes
