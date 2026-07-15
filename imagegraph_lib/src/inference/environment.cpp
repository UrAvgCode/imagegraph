#include <imagegraph/inference/environment.h>

#include <cassert>
#include <unordered_map>

namespace {
    Ort::Env* environment = nullptr;
    imagegraph::inference::DepthModel* depth_model = nullptr;
    imagegraph::inference::SegmentModel* segment_model = nullptr;
} // namespace

namespace imagegraph::inference {
    void init_environment(const Device device) {
        assert(!environment);
        environment = new Ort::Env(ORT_LOGGING_LEVEL_ERROR, "imagegraph");
        environment->DisableTelemetryEvents();

        if (device == Device::Cuda) {
            try {
                const auto memory_info = Ort::MemoryInfo("Cuda", OrtArenaAllocator, 0, OrtMemTypeDefault);
                const auto arena_config = Ort::ArenaCfg(0, -1, -1, -1);
                environment->CreateAndRegisterAllocatorV2("CUDAExecutionProvider", memory_info, {}, arena_config);
            } catch (const Ort::Exception& exception) {
                std::printf("Could not create shared CUDA allocator: %s\n", exception.what());
            }
        }

        depth_model = new DepthModel(device);
        segment_model = new SegmentModel(device);
    }

    void destroy_environment() {
        delete segment_model;
        delete depth_model;
        delete environment;
        environment = nullptr;
        depth_model = nullptr;
        segment_model = nullptr;
    }

    Ort::Env* get_environment() {
        assert(environment);
        return environment;
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
