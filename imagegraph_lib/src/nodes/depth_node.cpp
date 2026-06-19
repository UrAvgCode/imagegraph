#include <nodes/depth_node.h>

#include <compute/transfer.h>
#include <image/algorithm.h>
#include <image/preprocess.h>
#include <widgets/image_preview.h>
#include <widgets/indeterminate_progress_bar.h>

#include <algorithm>
#include <cstdio>
#include <utility>

namespace {
    const auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

    imagegraph::image::Image run_depth_estimation(imagegraph::image::Image input_image, Ort::Session* session,
                                                  const std::array<int, 2> tensor_size, const std::string& input_name,
                                                  const std::string& output_name) {

        input_image.resize(tensor_size[0], tensor_size[1]);
        auto input_tensor_values = imagegraph::image::image_to_tensor(input_image);

        const auto input_shape = std::array<int64_t, 4>{1, 3, tensor_size[1], tensor_size[0]};
        const auto input_tensor =
                Ort::Value::CreateTensor<float>(memory_info, input_tensor_values.data(), input_tensor_values.size(),
                                                input_shape.data(), input_shape.size());

        const char* input_names[] = {input_name.c_str()};
        const char* output_names[] = {output_name.c_str()};

        const auto output_tensors = session->Run(Ort::RunOptions(), input_names, &input_tensor, 1, output_names, 1);
        const auto output_data = output_tensors.front().GetTensorData<float>();

        auto output_image = imagegraph::image::Image(tensor_size[0], tensor_size[1], 1, output_data);
        imagegraph::image::normalize(output_image);
        return output_image;
    }
} // namespace

namespace imagegraph::nodes {
    DepthNode::DepthNode() :
        _output_size({518, 518}), _env(ORT_LOGGING_LEVEL_ERROR, "depth_anything"), _session(nullptr),
        _processing(false) {
        _input_pins.emplace_back(graph::Pin::Type::Texture, this);
        _output_pins.emplace_back(graph::Pin::Type::Mask, this);

        try {
            constexpr auto model_path = "models/depth_anything.onnx";
            auto session_options = Ort::SessionOptions();
            session_options.SetGraphOptimizationLevel(ORT_ENABLE_ALL);
            _session = Ort::Session(_env, model_path, session_options);

            const auto allocator = Ort::AllocatorWithDefaultOptions();
            _input_name = _session.GetInputNameAllocated(0, allocator).get();
            _output_name = _session.GetOutputNameAllocated(0, allocator).get();
        } catch (const std::exception& exception) {
            std::printf("%s\n", exception.what());
        }
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

                ImGui::PushItemWidth(input_width);
                ImGui::TextUnformatted("Size");
                ImGui::SameLine(label_width);
                if (ImGui::InputInt2("##output_size", _output_size.data())) {
                    _output_size[0] = std::clamp((_output_size[0] / 14) * 14, 14, 2072);
                    _output_size[1] = std::clamp((_output_size[1] / 14) * 14, 14, 2072);
                }
                if (ImGui::IsItemDeactivatedAfterEdit()) {
                    modified();
                }
                ImGui::PopItemWidth();

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
        _future = std::async(std::launch::async, run_depth_estimation, std::move(input_data), &_session, _output_size,
                             _input_name, _output_name);

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
