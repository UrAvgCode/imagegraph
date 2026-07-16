#pragma once

#include <imagegraph/inference/depth_model.h>
#include <imagegraph/inference/segment_model.h>

namespace imagegraph::inference {
    enum class Device { Auto, Cpu, Cuda };

    void init_environment(Device = Device::Auto);

    void destroy_environment();

    const Ort::Env& get_environment();

    const Ort::SessionOptions& get_session_options();

    const Ort::RunOptions& get_run_options();

    DepthModel* get_depth_model();

    SegmentModel* get_segment_model();
} // namespace imagegraph::inference
