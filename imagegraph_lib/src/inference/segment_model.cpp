#include <imagegraph/inference/segment_model.h>

#include <imagegraph/image/preprocess.h>
#include <imagegraph/inference/environment.h>

#include <cassert>
#include <cstdint>

namespace {
    constexpr int input_width = 1024;
    constexpr int input_height = 1024;
    const auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

    void sort_masks(std::array<imagegraph::image::Image, 3>& masks, std::array<float, 3>& iou_predictions) {
        if (iou_predictions[0] < iou_predictions[1]) {
            std::swap(iou_predictions[0], iou_predictions[1]);
            std::swap(masks[0], masks[1]);
        }
        if (iou_predictions[1] < iou_predictions[2]) {
            std::swap(iou_predictions[1], iou_predictions[2]);
            std::swap(masks[1], masks[2]);
        }
        if (iou_predictions[0] < iou_predictions[1]) {
            std::swap(iou_predictions[0], iou_predictions[1]);
            std::swap(masks[0], masks[1]);
        }
    }

    std::array<imagegraph::image::Image, 3> decoder_outputs_to_masks(const std::vector<Ort::Value>& outputs) {
        assert(outputs.size() == 2);
        const auto& masks_output = outputs[0];
        const auto& iou_output = outputs[1];

        const auto mask_shape = masks_output.GetTensorTypeAndShapeInfo().GetShape();
        const auto iou_shape = iou_output.GetTensorTypeAndShapeInfo().GetShape();
        assert(mask_shape.size() == 4 && iou_shape.size() == 2);

        const auto mask_count = static_cast<std::size_t>(mask_shape[1]);
        assert(mask_count == 3);

        const auto mask_height = static_cast<std::size_t>(mask_shape[2]);
        const auto mask_width = static_cast<std::size_t>(mask_shape[3]);

        const auto mask_data = masks_output.GetTensorData<float>();
        const auto mask_stride = mask_width * mask_height;

        auto masks = std::array<imagegraph::image::Image, 3>();
        for (std::size_t i = 0; i < masks.size(); ++i) {
            masks[i] = {static_cast<int>(mask_width), static_cast<int>(mask_height), 1, mask_data + i * mask_stride};
        }

        const auto iou_data = iou_output.GetTensorData<float>();
        auto iou_predictions = std::array{iou_data[0], iou_data[1], iou_data[2]};
        sort_masks(masks, iou_predictions);

        return masks;
    }
} // namespace

namespace imagegraph::inference {
    Ort::Value* DecoderInputs::data() { return &image_embed; }

    SegmentModel::SegmentModel() :
        _encoder_session(get_environment(), "models/segment_anything_encoder.onnx", get_session_options()),
        _decoder_session(get_environment(), "models/segment_anything_decoder.onnx", get_session_options()),
        _encoder_input_names(input_names(_encoder_session)), _encoder_output_names(output_names(_encoder_session)),
        _decoder_input_names(input_names(_decoder_session)), _decoder_output_names(output_names(_decoder_session)) {
        assert(_encoder_input_names.size() == 1);
        assert(_encoder_output_names.size() == 3);
        assert(_decoder_input_names.size() == 7);
        assert(_decoder_output_names.size() == 2);
    }

    DecoderInputs SegmentModel::encode(image::Image input_image) {
        try {
            input_image.resize(input_width, input_height);
            auto tensor_values = image::image_to_tensor(input_image);

            constexpr auto input_shape = std::array<std::int64_t, 4>{1, 3, input_height, input_width};
            const auto encoder_input_tensor = Ort::Value::CreateTensor<float>(
                    memory_info, tensor_values.data(), tensor_values.size(), input_shape.data(), input_shape.size());

            auto outputs = _encoder_session.Run(get_run_options(), _encoder_input_names.data(), &encoder_input_tensor,
                                                _encoder_input_names.size(), _encoder_output_names.data(),
                                                _encoder_output_names.size());

            return {.image_embed = std::move(outputs[2]),
                    .high_res_feats_0 = std::move(outputs[0]),
                    .high_res_feats_1 = std::move(outputs[1])};

        } catch (Ort::Exception& exception) {
            std::fprintf(stderr, "%s\n", exception.what());
            return {};
        }
    }

    std::array<image::Image, 3> SegmentModel::decode(DecoderInputs& inputs, const std::vector<PointPrompt>& prompts) {
        auto point_coords = std::vector<float>(prompts.size() * 2);
        auto point_labels = std::vector<float>(prompts.size());

        for (std::size_t i = 0; i < prompts.size(); ++i) {
            const auto& [position, label] = prompts[i];
            point_coords[i * 2] = position[0] * input_width;
            point_coords[i * 2 + 1] = position[1] * input_height;
            point_labels[i] = label == PointType::Positive ? 1.0f : 0.0f;
        }

        const auto prompts_size = static_cast<std::int64_t>(prompts.size());

        const auto coords_shape = std::array<std::int64_t, 3>{1, prompts_size, 2};
        auto coords_tensor = Ort::Value::CreateTensor<float>(memory_info, point_coords.data(), point_coords.size(),
                                                             coords_shape.data(), coords_shape.size());

        const auto labels_shape = std::array<std::int64_t, 2>{1, prompts_size};
        auto labels_tensor = Ort::Value::CreateTensor<float>(memory_info, point_labels.data(), point_labels.size(),
                                                             labels_shape.data(), labels_shape.size());

        auto mask_input = std::array<float, 256 * 256>{};
        constexpr auto mask_shape = std::array<std::int64_t, 4>{1, 1, 256, 256};
        auto mask_tensor = Ort::Value::CreateTensor<float>(memory_info, mask_input.data(), mask_input.size(),
                                                           mask_shape.data(), mask_shape.size());

        auto has_mask = std::array{0.0f};
        constexpr auto has_mask_shape = std::array<std::int64_t, 1>{1};
        Ort::Value has_mask_tensor = Ort::Value::CreateTensor<float>(memory_info, has_mask.data(), has_mask.size(),
                                                                     has_mask_shape.data(), has_mask_shape.size());

        inputs.point_coords = std::move(coords_tensor);
        inputs.point_labels = std::move(labels_tensor);
        inputs.mask_input = std::move(mask_tensor);
        inputs.has_mask_input = std::move(has_mask_tensor);

        const auto run_options = Ort::RunOptions();
        const auto outputs = _decoder_session.Run(get_run_options(), _decoder_input_names.data(), inputs.data(),
                                                  _decoder_input_names.size(), _decoder_output_names.data(),
                                                  _decoder_output_names.size());

        return decoder_outputs_to_masks(outputs);
    }
} // namespace imagegraph::inference
