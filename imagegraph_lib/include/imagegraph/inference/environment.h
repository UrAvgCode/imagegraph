#pragma once

#include <imagegraph/inference/depth_model.h>
#include <imagegraph/inference/segment_model.h>
#include <imagegraph/inference/session.h>

namespace imagegraph::inference {
    void init_environment(Device = Device::Cuda);

    void destroy_environment();

    Ort::Env* get_environment();

    DepthModel* get_depth_model();

    SegmentModel* get_segment_model();
} // namespace imagegraph::inference
