#include <imagegraph/inference/session.h>

namespace imagegraph::inference {
    Ort::Session create_session(const char* model_path, const Device device) {
        static auto env = new Ort::Env(ORT_LOGGING_LEVEL_ERROR, "imagegraph");

        auto options = Ort::SessionOptions();
        options.SetGraphOptimizationLevel(ORT_ENABLE_ALL);

        if (device == Device::Cuda) {
            const auto provider_options = OrtCUDAProviderOptions();
            try {
                options.AppendExecutionProvider_CUDA(provider_options);
            } catch (const Ort::Exception& exception) {
                std::printf("CUDA provider unavailable, falling back to CPU: %s\n", exception.what());
            }
        }

        return {*env, model_path, options};
    }
} // namespace imagegraph::inference
