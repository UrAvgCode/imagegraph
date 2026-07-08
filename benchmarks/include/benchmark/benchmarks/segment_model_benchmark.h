#pragma once

#include <benchmark/helper/input_node.h>

#include <imagegraph/inference/session.h>

#include <array>

namespace imagegraph::benchmark {
    class SegmentModelBenchmark {
    public:
        SegmentModelBenchmark();

        void run();

    private:
        std::array<std::uint64_t, 2> run_model(inference::Device);

        std::filesystem::path _output_root;
        InputNode _input_node;
    };
} // namespace imagegraph::benchmark
