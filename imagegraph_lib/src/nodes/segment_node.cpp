#define IMGUI_DEFINE_MATH_OPERATORS

#include <imagegraph/nodes/segment_node.h>

#include <imagegraph/compute/transfer.h>
#include <imagegraph/image/image.h>
#include <imagegraph/inference/environment.h>
#include <imagegraph/widgets/image_preview.h>
#include <imagegraph/widgets/indeterminate_progress_bar.h>

#include <shader/segmentation_threshold.h>

#include <algorithm>
#include <utility>

namespace imagegraph::nodes {
    SegmentNode::SegmentNode() :
        _prompts_modified(false), _mask_index(0), _mask_index_modified(false), _threshold(0.0f),
        _threshold_modified(false), _processing(false) {
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

            constexpr auto preview_size = ImVec2{150.0f, 150.0f};
            constexpr auto panel_width = preview_size.x;

            ImGui::SameLine();
            ImGui::BeginGroup();
            {
                ImGui::BeginDisabled(_prompts.empty());
                if (ImGui::Button("Undo") && !_prompts.empty()) {
                    _prompts.pop_back();
                    _prompts_modified = true;
                }
                ImGui::SameLine();
                if (ImGui::Button("Clear") && !_prompts.empty()) {
                    _prompts.clear();
                    _prompts_modified = true;
                }
                ImGui::EndDisabled();

                widgets::indeterminate_progress_bar(_processing, {panel_width, 13.0f});

                const auto input_texture = _input_pins[0].texture();
                const auto input_id = !input_texture ? GLuint{0} : input_texture->id();
                const auto input_size = !input_texture ? ImVec2{}
                                                       : ImVec2{static_cast<float>(input_texture->width()),
                                                                static_cast<float>(input_texture->height())};

                if (widgets::prompt_image_preview(input_id, input_size, preview_size, _prompts)) {
                    _prompts_modified = true;
                }
            }
            ImGui::EndGroup();

            ImGui::SameLine();
            ImGui::BeginGroup();
            {
                ImGui::AlignTextToFramePadding();
                ImGui::TextUnformatted("Threshold");

                const auto threshold_label_width = ImGui::GetItemRectSize().x;
                const auto threshold_input_width =
                        panel_width - threshold_label_width - ImGui::GetStyle().ItemSpacing.x;

                ImGui::SameLine();
                ImGui::SetNextItemWidth(threshold_input_width);

                if (ImGui::DragFloat("##threshold", &_threshold, 0.01, 0.0f, 0.0f, "%.2f")) {
                    _threshold_modified = true;
                }

                auto mask_label = std::array<char, 16>{};
                std::snprintf(mask_label.data(), mask_label.size(), "Mask %d / 3", _mask_index + 1);

                const auto row_origin = ImGui::GetCursorPos();
                const auto label_size = ImGui::CalcTextSize(mask_label.data());
                const auto arrow_size = ImGui::GetFrameHeight();

                const auto previous_position = ImVec2(row_origin.x, row_origin.y);
                const auto label_position = ImVec2(row_origin.x + (panel_width - label_size.x) * 0.5f,
                                                   row_origin.y + (arrow_size - label_size.y) * 0.5f);
                const auto next_position = ImVec2(row_origin.x + panel_width - arrow_size, row_origin.y);

                ImGui::SetCursorPos(previous_position);
                if (ImGui::ArrowButton("##previous", ImGuiDir_Left)) {
                    _mask_index = (_mask_index + 3 - 1) % 3;
                    _mask_index_modified = true;
                }

                ImGui::SetCursorPos(label_position);
                ImGui::TextUnformatted(mask_label.data());

                ImGui::SetCursorPos(next_position);
                if (ImGui::ArrowButton("##next", ImGuiDir_Right)) {
                    _mask_index = (_mask_index + 1) % 3;
                    _mask_index_modified = true;
                }

                const auto mask_size = ImVec2(static_cast<float>(_mask.width()), static_cast<float>(_mask.height()));
                widgets::image_preview(_mask.id(), mask_size, preview_size);
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
            _prompts_modified = true;
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
            _future = std::async(std::launch::async, &inference::SegmentModel::encode, inference::get_segment_model(),
                                 std::move(input_data));

            _processing = true;
            return;
        }

        if (_prompts_modified && _decoder_inputs.image_embed != nullptr) {
            _prompts_modified = false;
            _masks = inference::get_segment_model()->decode(_decoder_inputs, _prompts);
            _mask_index_modified = true;
        }

        if (const auto& mask = _masks[_mask_index]; _mask_index_modified && mask.size() != 0) {
            _mask_index_modified = false;
            compute::upload_mask(mask, &_logits_mask);
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
        auto prompts_json = nlohmann::json::array();

        for (const auto& [position, type]: _prompts) {
            prompts_json.push_back(
                    {{"position", position}, {"type", type == inference::PointType::Positive ? "+" : "-"}});
        }

        return {{"prompts", prompts_json}, {"mask_index", _mask_index}, {"threshold", _threshold}};
    }

    void SegmentNode::deserialize(const nlohmann::json& json) {
        if (json.contains("mask_index") && json["mask_index"].is_number()) {
            _mask_index = json["mask_index"].get<int>();
            _mask_index = std::clamp(_mask_index, 0, 2);
        }

        if (json.contains("threshold") && json["threshold"].is_number()) {
            _threshold = json["threshold"].get<float>();
        }

        if (json.contains("prompts") && json["prompts"].is_array()) {
            const auto& prompts_json = json.at("prompts");

            auto prompts = std::vector<inference::PointPrompt>();
            prompts.reserve(prompts_json.size());

            for (const auto& prompt_json: prompts_json) {
                const auto position = prompt_json.at("position").get<std::array<float, 2>>();
                const auto label_string = prompt_json.at("type").get<std::string>();

                auto type = inference::PointType{};
                if (label_string == "+") {
                    type = inference::PointType::Positive;
                } else if (label_string == "-") {
                    type = inference::PointType::Negative;
                } else {
                    continue;
                }

                prompts.emplace_back(position, type);
            }

            _prompts = std::move(prompts);
        }

        modified();
    }
} // namespace imagegraph::nodes
