#pragma once

#include <benchmark/benchmarks/benchmark.h>

#include <imagegraph/nodes/gaussian_blur_node.h>

namespace imagegraph::benchmark {
    class BlurBenchmark final : public Benchmark {
    public:
        BlurBenchmark();

    protected:
        void execute_cpu(const image::Image&, image::Image&) override;
        void execute_cpu_parallel(const image::Image&, image::Image&) override;

    private:
        nodes::GaussianBlurNode* _blur_node;
    };
} // namespace imagegraph::benchmark
