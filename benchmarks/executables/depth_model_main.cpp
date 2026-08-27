#include <benchmark/benchmarks/depth_model_benchmark.h>

int main() {
    auto depth_model_benchmark = imagegraph::benchmark::DepthModelBenchmark();
    depth_model_benchmark.run();
}
