#pragma once

#include <benchmark/helper/input_node.h>

#include <imagegraph/image/image.h>
#include <imagegraph/inference/environment.h>

#include <array>

namespace imagegraph::benchmark {
    class SegmentModelBenchmark {
    public:
        SegmentModelBenchmark();

        void run();

    private:
        std::array<std::uint64_t, 2> run_model(inference::Device) const;

        std::filesystem::path _output_root;
        image::Image _image;
    };
} // namespace imagegraph::benchmark
