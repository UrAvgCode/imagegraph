#include <imagegraph/inference/depth_model.h>

#include <imagegraph/image/algorithm.h>
#include <imagegraph/image/preprocess.h>

#include <cassert>

namespace {
    const auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
}

namespace imagegraph::inference {
    DepthModel::DepthModel(const Device device) :
        _session(create_session("models/depth_anything.onnx", device)), _input_names(input_names(_session)),
        _output_names(output_names(_session)) {
        assert(_input_names.size() == 1);
        assert(_output_names.size() == 1);
    }

    image::Image DepthModel::run(image::Image input_image, const std::array<int, 2> tensor_size) {
        try {
            input_image.resize(tensor_size[0], tensor_size[1]);
            auto input_tensor_values = imagegraph::image::image_to_tensor(input_image);

            const auto input_shape = std::array<int64_t, 4>{1, 3, tensor_size[1], tensor_size[0]};
            const auto input_tensor =
                    Ort::Value::CreateTensor<float>(memory_info, input_tensor_values.data(), input_tensor_values.size(),
                                                    input_shape.data(), input_shape.size());


            const auto output_tensors = _session.Run(Ort::RunOptions(), _input_names.data(), &input_tensor,
                                                     _input_names.size(), _output_names.data(), _output_names.size());


            const auto output_data = output_tensors.front().GetTensorData<float>();

            auto output_image = image::Image(tensor_size[0], tensor_size[1], 1, output_data);
            imagegraph::image::normalize(output_image);
            return output_image;

        } catch (Ort::Exception& exception) {
            std::fprintf(stderr, "%s\n", exception.what());
            return {};
        }
    }
} // namespace imagegraph::inference
