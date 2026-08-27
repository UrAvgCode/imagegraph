#include <benchmark/benchmarks/segment_model_benchmark.h>

int main() {
    auto segment_model_benchmark = imagegraph::benchmark::SegmentModelBenchmark();
    segment_model_benchmark.run();
}
