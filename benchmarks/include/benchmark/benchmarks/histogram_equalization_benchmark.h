#pragma once

#include <benchmark/benchmarks/benchmark.h>

#include <imagegraph/nodes/histogram_equalization_node.h>

namespace imagegraph::benchmark {
    class HistogramEqualizationBenchmark final : public Benchmark {
    public:
        HistogramEqualizationBenchmark();

    protected:
        void execute_cpu(const image::Image&, image::Image&) override;
        void execute_cpu_parallel(const image::Image&, image::Image&) override;

    private:
        nodes::HistogramEqualizationNode* _histogram_node;
    };
} // namespace imagegraph::benchmark
