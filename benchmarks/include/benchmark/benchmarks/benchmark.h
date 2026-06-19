#pragma once

#include <benchmark/helper/input_node.h>

#include <imagegraph/graph/graph.h>

#include <filesystem>

namespace imagegraph::benchmark {
    struct BenchmarkResult {
        image::Image output;
        std::uint64_t min{};
        std::uint64_t max{};
        std::uint64_t average{};
    };

    class Benchmark {
    public:
        explicit Benchmark(const char*);
        virtual ~Benchmark() = default;

        void run();

    protected:
        BenchmarkResult run_gpu();
        BenchmarkResult run_cpu();
        BenchmarkResult run_cpu_parallel();

        virtual void execute_cpu(const image::Image&, image::Image&) = 0;
        virtual void execute_cpu_parallel(const image::Image&, image::Image&) = 0;

        const char* _name;
        std::filesystem::path _output_root;

        graph::Graph _graph;
        InputNode* _input_node;
        graph::OutputPin* _output_pin;
    };
} // namespace imagegraph::benchmark
