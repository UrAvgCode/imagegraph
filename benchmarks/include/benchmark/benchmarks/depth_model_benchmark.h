#pragma once

#include <benchmark/helper/input_node.h>

#include <imagegraph/inference/environment.h>

namespace imagegraph::benchmark {
    class DepthModelBenchmark {
    public:
        DepthModelBenchmark();

        void run();

    private:
        std::uint64_t run_model(inference::Device);

        std::filesystem::path _output_root;
        InputNode _input_node;
    };
} // namespace imagegraph::benchmark
