#include <imagegraph/inference/segment_model.h>

#include <imagegraph/image/preprocess.h>
#include <imagegraph/inference/environment.h>

#include <cassert>

namespace {
    constexpr int tensor_width = 1024;
    constexpr int tensor_height = 1024;
    const auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
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
            input_image.resize(tensor_width, tensor_height);
            auto tensor_values = image::image_to_tensor(input_image);

            constexpr auto input_shape = std::array<int64_t, 4>{1, 3, tensor_height, tensor_width};
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

    image::Image SegmentModel::decode(DecoderInputs& inputs, const std::array<float, 2> uv) {
        auto point_coords = std::array{uv[0] * tensor_width, uv[1] * tensor_height};
        constexpr auto coords_shape = std::array<int64_t, 3>{1, 1, 2};
        auto coords_tensor = Ort::Value::CreateTensor<float>(memory_info, point_coords.data(), point_coords.size(),
                                                             coords_shape.data(), coords_shape.size());

        auto point_labels = std::array{1.0f};
        constexpr auto labels_shape = std::array<int64_t, 2>{1, 1};
        auto labels_tensor = Ort::Value::CreateTensor<float>(memory_info, point_labels.data(), point_labels.size(),
                                                             labels_shape.data(), labels_shape.size());

        auto mask_input = std::array<float, 256 * 256>{};
        constexpr auto mask_shape = std::array<int64_t, 4>{1, 1, 256, 256};
        auto mask_tensor = Ort::Value::CreateTensor<float>(memory_info, mask_input.data(), mask_input.size(),
                                                           mask_shape.data(), mask_shape.size());

        auto has_mask = std::array{0.0f};
        constexpr auto has_mask_shape = std::array<int64_t, 1>{1};
        Ort::Value has_mask_tensor = Ort::Value::CreateTensor<float>(memory_info, has_mask.data(), has_mask.size(),
                                                                     has_mask_shape.data(), has_mask_shape.size());

        inputs.point_coords = std::move(coords_tensor);
        inputs.point_labels = std::move(labels_tensor);
        inputs.mask_input = std::move(mask_tensor);
        inputs.has_mask_input = std::move(has_mask_tensor);

        const auto run_options = Ort::RunOptions();
        auto outputs = _decoder_session.Run(get_run_options(), _decoder_input_names.data(), inputs.data(),
                                            _decoder_input_names.size(), _decoder_output_names.data(),
                                            _decoder_output_names.size());

        const auto mask_ptr = outputs[0].GetTensorMutableData<float>();
        const auto mask_info = outputs[0].GetTensorTypeAndShapeInfo();
        const auto mask_dimensions = mask_info.GetShape();

        const auto mask_width = static_cast<int>(mask_dimensions[3]);
        const auto mask_height = static_cast<int>(mask_dimensions[2]);

        return {mask_width, mask_height, 1, mask_ptr};
    }
} // namespace imagegraph::inference
