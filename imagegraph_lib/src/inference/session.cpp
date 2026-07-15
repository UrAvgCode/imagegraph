#include <imagegraph/inference/session.h>

#include <imagegraph/inference/environment.h>

namespace imagegraph::inference {
    Ort::Session create_session(const char* model_path, const Device device) {
        auto options = Ort::SessionOptions();
        options.SetGraphOptimizationLevel(ORT_ENABLE_ALL);

        if (device == Device::Cuda) {
            const auto provider_options = OrtCUDAProviderOptions();
            try {
                options.AppendExecutionProvider_CUDA(provider_options);
                options.AddConfigEntry("session.use_env_allocators", "1");
            } catch (const Ort::Exception& exception) {
                std::printf("CUDA provider unavailable, falling back to CPU: %s\n", exception.what());
            }
        }

        return {*get_environment(), model_path, options};
    }
} // namespace imagegraph::inference
