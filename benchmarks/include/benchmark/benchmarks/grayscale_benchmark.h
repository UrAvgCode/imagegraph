#pragma once

#include <benchmark/benchmarks/benchmark.h>

#include <imagegraph/nodes/color_intensity_node.h>

namespace imagegraph::benchmark {
    class GrayscaleBenchmark final : public Benchmark {
    public:
        GrayscaleBenchmark();

    protected:
        void execute_cpu(const image::Image&, image::Image&) override;
        void execute_cpu_parallel(const image::Image&, image::Image&) override;

    private:
        nodes::ColorIntensityNode* _grayscale_node;
    };
} // namespace imagegraph::benchmark
