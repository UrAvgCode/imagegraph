#define IMGUI_DEFINE_MATH_OPERATORS

#include <nodes/segment_node.h>

#include <compute/transfer.h>
#include <image/image.h>
#include <image/preprocess.h>
#include <widgets/image_preview.h>
#include <widgets/indeterminate_progress_bar.h>

#include <algorithm>
#include <cstdio>
#include <utility>

namespace {
    constexpr int tensor_width = 1024;
    constexpr int tensor_height = 1024;

    const auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

    imagegraph::nodes::DecoderInputs run_encoder(imagegraph::image::Image input_image, Ort::Session* encoder_session,
                                                 const std::vector<const char*>& input_names,
                                                 const std::vector<const char*>& output_names) {

        input_image.resize(tensor_width, tensor_height);
        auto tensor_values = imagegraph::image::image_to_tensor(input_image);

        constexpr auto input_shape = std::array<int64_t, 4>{1, 3, tensor_height, tensor_width};
        const auto encoder_input_tensor = Ort::Value::CreateTensor<float>(
                memory_info, tensor_values.data(), tensor_values.size(), input_shape.data(), input_shape.size());

        auto outputs = encoder_session->Run(Ort::RunOptions{nullptr}, input_names.data(), &encoder_input_tensor,
                                            input_names.size(), output_names.data(), output_names.size());

        return {.image_embed = std::move(outputs[2]),
                .high_res_feats_0 = std::move(outputs[0]),
                .high_res_feats_1 = std::move(outputs[1])};
    }

    imagegraph::image::Image run_decoder(imagegraph::nodes::DecoderInputs* inputs, const std::array<float, 2> uv,
                                         Ort::Session* decoder_session, const std::vector<const char*>& input_names,
                                         const std::vector<const char*>& output_names) {

        auto point_coords = std::array{uv[0] * tensor_width, uv[1] * tensor_height};
        constexpr auto coords_shape = std::array<int64_t, 3>{1, 1, 2};
        auto coords_tensor = Ort::Value::CreateTensor<float>(memory_info, point_coords.data(), point_coords.size(),
                                                             coords_shape.data(), coords_shape.size());

        auto point_labels = std::array{1.0f};
        constexpr auto labels_shape = std::array<int64_t, 2>{1, 1};
        auto labels_tensor = Ort::Value::CreateTensor<float>(memory_info, point_labels.data(), point_labels.size(),
                                                             labels_shape.data(), labels_shape.size());

        auto mask_input = std::vector(1 * 1 * 256 * 256, 0.0f);
        constexpr auto mask_shape = std::array<int64_t, 4>{1, 1, 256, 256};
        auto mask_tensor = Ort::Value::CreateTensor<float>(memory_info, mask_input.data(), mask_input.size(),
                                                           mask_shape.data(), mask_shape.size());

        auto has_mask = std::array{0.0f};
        constexpr auto has_mask_shape = std::array<int64_t, 1>{1};
        Ort::Value has_mask_tensor = Ort::Value::CreateTensor<float>(memory_info, has_mask.data(), has_mask.size(),
                                                                     has_mask_shape.data(), has_mask_shape.size());

        inputs->point_coords = std::move(coords_tensor);
        inputs->point_labels = std::move(labels_tensor);
        inputs->mask_input = std::move(mask_tensor);
        inputs->has_mask_input = std::move(has_mask_tensor);

        auto outputs = decoder_session->Run(Ort::RunOptions{nullptr}, input_names.data(),
                                            reinterpret_cast<Ort::Value*>(inputs), input_names.size(),
                                            output_names.data(), output_names.size());

        const auto mask_ptr = outputs[0].GetTensorMutableData<float>();
        const auto mask_info = outputs[0].GetTensorTypeAndShapeInfo();
        const auto mask_dimensions = mask_info.GetShape();

        const auto mask_width = static_cast<int>(mask_dimensions[3]);
        const auto mask_height = static_cast<int>(mask_dimensions[2]);

        auto output_image = imagegraph::image::Image(mask_width, mask_height, 4);
        for (std::size_t i = 0; i < mask_width * mask_height; ++i) {
            const auto value = static_cast<float>(mask_ptr[i] > 0.0f);
            output_image.data()[i * 4 + 0] = value;
            output_image.data()[i * 4 + 1] = value;
            output_image.data()[i * 4 + 2] = value;
            output_image.data()[i * 4 + 3] = 1.0f;
        }

        return output_image;
    }
} // namespace

namespace imagegraph::nodes {
    SegmentNode::SegmentNode() :
        _uv({0.5f, 0.5f}), _point_modified(false), _env(ORT_LOGGING_LEVEL_ERROR, "segment_anything"),
        _encoder_session(nullptr), _decoder_session(nullptr), _processing(false) {
        _input_pins.emplace_back(graph::PinType::Texture, this);
        _output_pins.emplace_back(graph::PinType::Texture, this);

        try {
            constexpr auto encoder_path = "models/segment_anything_encoder.onnx";
            constexpr auto decoder_path = "models/segment_anything_decoder.onnx";

            auto session_options = Ort::SessionOptions();
            session_options.SetGraphOptimizationLevel(ORT_ENABLE_ALL);

            _encoder_session = Ort::Session(_env, encoder_path, session_options);
            _decoder_session = Ort::Session(_env, decoder_path, session_options);

            _encoder_input_name_storage = _encoder_session.GetInputNames();
            _encoder_output_name_storage = _encoder_session.GetOutputNames();

            _decoder_input_name_storage = _decoder_session.GetInputNames();
            _decoder_output_name_storage = _decoder_session.GetOutputNames();

            std::ranges::transform(_encoder_input_name_storage, std::back_inserter(_encoder_input_names),
                                   &std::string::c_str);
            std::ranges::transform(_encoder_output_name_storage, std::back_inserter(_encoder_output_names),
                                   &std::string::c_str);

            std::ranges::transform(_decoder_input_name_storage, std::back_inserter(_decoder_input_names),
                                   &std::string::c_str);
            std::ranges::transform(_decoder_output_name_storage, std::back_inserter(_decoder_output_names),
                                   &std::string::c_str);

        } catch (const std::exception& exception) {
            std::printf("%s\n", exception.what());
        }
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
                ImGui::PushItemWidth(200.0f);
                if (ImGui::InputFloat2("UV", _uv.data())) {
                    _uv[0] = std::clamp(_uv[0], 0.0f, 1.0f);
                    _uv[1] = std::clamp(_uv[1], 0.0f, 1.0f);
                    _point_modified = true;
                }
                ImGui::PopItemWidth();

                widgets::indeterminate_progress_bar(_processing);

                constexpr auto image_size = ImVec2(100, 100);
                const auto input_texture = std::get<compute::Texture*>(_input_pins[0].get_value());

                if (input_texture && input_texture->id() != 0) {
                    ImGui::Image(input_texture->id(), image_size);
                    const auto image_min = ImGui::GetItemRectMin();

                    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        const auto mouse_pos = ImGui::GetMousePos();
                        const auto local_pos = mouse_pos - image_min;

                        const auto uv = local_pos / image_size;
                        _uv = {uv.x, uv.y};

                        _point_modified = true;
                    }

                    const auto draw_list = ImGui::GetWindowDrawList();
                    const float px = image_min.x + _uv[0] * image_size.x;
                    const float py = image_min.y + _uv[1] * image_size.y;
                    draw_list->AddCircleFilled(ImVec2(px, py), 2.0f, IM_COL32(255, 0, 0, 255));
                } else {
                    widgets::image_preview(nullptr, {0.0f, 0.0f}, image_size);
                }

                ImGui::SameLine();

                const auto texture_size =
                        ImVec2(static_cast<float>(_texture.width()), static_cast<float>(_texture.height()));
                widgets::image_preview(_texture.id(), texture_size, ImVec2(100, 100));
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
            _decoder_inputs = _future.get();
            _processing = false;
            _point_modified = true;
        }

        if (!_processing && _point_modified && _decoder_inputs.image_embed != nullptr) {
            const auto result =
                    run_decoder(&_decoder_inputs, _uv, &_decoder_session, _decoder_input_names, _decoder_output_names);

            if (result.width() && result.height()) {
                compute::upload_image(result, &_texture);
                _output_pins[0].set_value(&_texture);
            }

            _point_modified = false;
        }

        if (!_modified || _processing) {
            return;
        }
        _modified = false;

        const auto input_texture = std::get<compute::Texture*>(_input_pins[0].get_value());
        if (!input_texture || input_texture->id() == 0) {
            _texture = compute::Texture();
            _output_pins[0].set_value({});
            return;
        }

        auto input_data = compute::download_texture(input_texture);
        _future = std::async(std::launch::async, run_encoder, std::move(input_data), &_encoder_session,
                             _encoder_input_names, _encoder_output_names);

        _processing = true;
    }

    nlohmann::json SegmentNode::serialize() const { return {{"uv_x", _uv[0]}, {"uv_y", _uv[1]}}; }

    void SegmentNode::deserialize(const nlohmann::json& json) {
        if (json.contains("uv_x") && json["uv_x"].is_number()) {
            _uv[0] = json["uv_x"].get<float>();
        }
        if (json.contains("uv_y") && json["uv_y"].is_number()) {
            _uv[1] = json["uv_y"].get<float>();
        }
        modified();
    }
} // namespace imagegraph::nodes
