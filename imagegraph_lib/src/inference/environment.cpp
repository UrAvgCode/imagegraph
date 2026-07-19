#include <imagegraph/inference/environment.h>

#include <onnxruntime_run_options_config_keys.h>
#include <onnxruntime_session_options_config_keys.h>

#include <cassert>

namespace {
    Ort::Env* environment = nullptr;
    Ort::SessionOptions* session_options = nullptr;
    Ort::RunOptions* run_options = nullptr;

    imagegraph::inference::DepthModel* depth_model = nullptr;
    imagegraph::inference::SegmentModel* segment_model = nullptr;

    imagegraph::inference::Device get_auto_device() {
        try {
            auto options = Ort::SessionOptions();
            const auto cuda_options = OrtCUDAProviderOptions();
            options.AppendExecutionProvider_CUDA(cuda_options);

            std::printf("using cuda execution provider.\n");
            return imagegraph::inference::Device::Cuda;
        } catch (...) {
            std::printf("cuda execution provider unavailable, falling back to cpu.\n");
            return imagegraph::inference::Device::Cpu;
        }
    }
} // namespace

namespace imagegraph::inference {
    void init_environment(Device device) {
        environment = new Ort::Env(ORT_LOGGING_LEVEL_ERROR, "imagegraph");
        environment->DisableTelemetryEvents();

        const auto cpu_memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        const auto cpu_arena_config = Ort::ArenaCfg(0, -1, -1, -1);
        environment->CreateAndRegisterAllocator(cpu_memory_info, cpu_arena_config);

        session_options = new Ort::SessionOptions();
        session_options->SetGraphOptimizationLevel(ORT_ENABLE_ALL);
        session_options->AddConfigEntry(kOrtSessionOptionsConfigUseEnvAllocators, "1");

        run_options = new Ort::RunOptions();

        if (device == Device::Auto) {
            device = get_auto_device();
        }

        if (device == Device::Cuda) {
            const auto provider_options = OrtCUDAProviderOptions();
            session_options->AppendExecutionProvider_CUDA(provider_options);

            run_options->AddConfigEntry(kOrtRunOptionsConfigEnableMemoryArenaShrinkage, "gpu:0");

            const auto memory_info = Ort::MemoryInfo("Cuda", OrtArenaAllocator, 0, OrtMemTypeDefault);
            const auto arena_config = Ort::ArenaCfg(0, -1, -1, -1);
            environment->CreateAndRegisterAllocatorV2("CUDAExecutionProvider", memory_info, {}, arena_config);
        }

        depth_model = new DepthModel();
        segment_model = new SegmentModel();
    }

    void destroy_environment() {
        delete segment_model;
        delete depth_model;
        delete session_options;
        delete run_options;
        delete environment;
        environment = nullptr;
        run_options = nullptr;
        session_options = nullptr;
        depth_model = nullptr;
        segment_model = nullptr;
    }

    const Ort::Env& get_environment() {
        assert(environment);
        return *environment;
    }

    const Ort::SessionOptions& get_session_options() {
        assert(session_options);
        return *session_options;
    }

    const Ort::RunOptions& get_run_options() {
        assert(run_options);
        return *run_options;
    }

    DepthModel* get_depth_model() {
        assert(depth_model);
        return depth_model;
    }

    SegmentModel* get_segment_model() {
        assert(segment_model);
        return segment_model;
    }
} // namespace imagegraph::inference
