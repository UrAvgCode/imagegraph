#include <nodes/depth_node.h>

#include <compute/transfer.h>
#include <image/algorithm.h>
#include <image/preprocess.h>
#include <widgets/image_preview.h>
#include <widgets/indeterminate_progress_bar.h>

#include <cstdio>
#include <utility>

namespace {
    const auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

    imagegraph::image::Image run_depth_estimation(imagegraph::image::Image input_image, Ort::Session* session,
                                                  const std::string& input_name, const std::string& output_name) {
        constexpr std::size_t tensor_width = 518;
        constexpr std::size_t tensor_height = 518;

        input_image.resize(tensor_width, tensor_height);
        auto input_tensor_values = imagegraph::image::image_to_tensor(input_image);

        constexpr auto input_shape = std::array<int64_t, 4>{1, 3, tensor_height, tensor_width};
        const auto input_tensor =
                Ort::Value::CreateTensor<float>(memory_info, input_tensor_values.data(), input_tensor_values.size(),
                                                input_shape.data(), input_shape.size());

        const char* input_names[] = {input_name.c_str()};
        const char* output_names[] = {output_name.c_str()};

        const auto output_tensors = session->Run(Ort::RunOptions(), input_names, &input_tensor, 1, output_names, 1);
        const auto output_data = output_tensors.front().GetTensorData<float>();

        auto normalized = imagegraph::image::Image(tensor_width, tensor_height, 1, output_data);
        imagegraph::image::normalize(normalized);

        auto output_image = imagegraph::image::Image(tensor_width, tensor_height, 4);
        for (std::size_t i = 0; i < normalized.size(); ++i) {
            output_image[i * 4 + 0] = normalized[i];
            output_image[i * 4 + 1] = normalized[i];
            output_image[i * 4 + 2] = normalized[i];
            output_image[i * 4 + 3] = 1.0f;
        }

        return output_image;
    }
} // namespace

namespace imagegraph::nodes {
    DepthNode::DepthNode() : _env(ORT_LOGGING_LEVEL_ERROR, "depth_anything"), _session(nullptr), _processing(false) {
        _input_pins.emplace_back(graph::PinType::Texture, this);
        _output_pins.emplace_back(graph::PinType::Texture, this);

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
                widgets::indeterminate_progress_bar(_processing);

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
        }
        ImGui::PopID();
        ax::NodeEditor::EndNode();
    }

    void DepthNode::evaluate() {
        if (_processing && _future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
            const auto result = _future.get();

            if (result.width() && result.height()) {
                compute::upload_image(result, &_texture);
                _output_pins[0].set_value(&_texture);
            }

            _processing = false;
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
        _future = std::async(std::launch::async, run_depth_estimation, std::move(input_data), &_session, _input_name,
                             _output_name);

        _processing = true;
    }

    nlohmann::json DepthNode::serialize() const { return {}; }

    void DepthNode::deserialize(const nlohmann::json&) {}
} // namespace imagegraph::nodes
