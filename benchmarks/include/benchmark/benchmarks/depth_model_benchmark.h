#pragma once

#include <benchmark/helper/input_node.h>

#include <imagegraph/image/image.h>
#include <imagegraph/inference/environment.h>

namespace imagegraph::benchmark {
    class DepthModelBenchmark {
    public:
        DepthModelBenchmark();

        void run();

    private:
        std::uint64_t run_model(inference::Device) const;

        std::filesystem::path _output_root;
        image::Image _image;
    };
} // namespace imagegraph::benchmark
