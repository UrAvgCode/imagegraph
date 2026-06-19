#include <benchmark/benchmarks/benchmark.h>

#include <imagegraph/compute/transfer.h>
#include <imagegraph/image/io.h>

#include <cmath>
#include <format>
#include <fstream>
#include <iostream>
#include <limits>

namespace {
    constexpr std::size_t iterations = 100;

    struct BenchmarkAbsDiff {
        float min = std::numeric_limits<float>::max();
        float max = 0.0f;
        float average = 0.0f;
    };

    BenchmarkAbsDiff abs_diff(const imagegraph::image::Image& input_one, const imagegraph::image::Image& input_two) {
        assert(input_one.size() == input_two.size());
        auto result = BenchmarkAbsDiff();

        float total = 0.0f;
        for (std::size_t i = 0; i < input_one.size(); ++i) {
            float abs_diff = std::abs(input_one[i] - input_two[i]);
            result.min = std::min(result.min, abs_diff);
            result.max = std::max(result.max, abs_diff);
            total += abs_diff;
        }

        result.average = total / static_cast<float>(input_one.size());
        return result;
    }
} // namespace

namespace imagegraph::benchmark {
    Benchmark::Benchmark(const char* name) : _name(name), _output_pin(nullptr) {
        _output_root = std::filesystem::path("outputs") / _name;
        std::filesystem::create_directories(_output_root);

        const auto input_node = _graph.add_node(std::make_unique<InputNode>());
        _input_node = dynamic_cast<InputNode*>(input_node);
    }

    void Benchmark::run() {
        auto file = std::ofstream(_output_root / "results.csv");
        file << "size,gpu,cpu,cpu_parallel,abs_diff_max\n";

        std::printf("running %s benchmark\n", _name);
        for (int size = 32; size <= 8192; size *= 2) {
            std::printf("benchmarking %dx%d...\n", size, size);
            _input_node->set_size(size, size);

            const auto gpu_result = run_gpu();
            const auto cpu_result = run_cpu();
            const auto cpu_parallel_result = run_cpu_parallel();

            auto abs_diff_result = abs_diff(gpu_result.output, cpu_result.output);

            file << size << ',' << gpu_result.average << ',' << cpu_result.average << ',' << cpu_parallel_result.average
                 << ',' << abs_diff_result.max << '\n';
        }
    }

    BenchmarkResult Benchmark::run_gpu() {
        GLuint query = 0;
        glGenQueries(1, &query);

        _graph.evaluate();
        glFinish();

        std::uint64_t min = std::numeric_limits<std::uint64_t>::max();
        std::uint64_t max = 0;
        std::uint64_t total = 0;

        for (std::size_t i = 0; i < iterations; ++i) {
            for (const auto node: _graph.nodes()) {
                node->modified();
            }

            glFinish();
            glBeginQuery(GL_TIME_ELAPSED, query);

            _graph.evaluate();

            glEndQuery(GL_TIME_ELAPSED);

            GLuint64 elapsed = 0;
            glGetQueryObjectui64v(query, GL_QUERY_RESULT, &elapsed);

            min = std::min(min, elapsed);
            max = std::max(max, elapsed);
            total += elapsed;
        }

        glDeleteQueries(1, &query);

        const auto average = total / iterations;

        const auto texture = _output_pin->texture();
        const auto output = compute::download_texture(texture);

        const auto path = _output_root / "gpu" / std::format("{}x{}.png", output.width(), output.height());
        std::filesystem::create_directories(path.parent_path());
        imagegraph::image::save_to_file(output, path);

        return {output, min, max, average};
    }

    BenchmarkResult Benchmark::run_cpu() {
        const auto input = _input_node->image();
        auto output = image::Image(input.width(), input.height(), input.channels());

        std::uint64_t min = std::numeric_limits<std::uint64_t>::max();
        std::uint64_t max = 0;
        std::uint64_t total = 0;

        for (std::size_t i = 0; i < iterations; ++i) {
            const auto start = std::chrono::steady_clock::now();

            execute_cpu(input, output);

            const auto end = std::chrono::steady_clock::now();
            const auto elapsed = std::chrono::duration<std::uint64_t, std::nano>(end - start).count();

            min = std::min(min, elapsed);
            max = std::max(max, elapsed);
            total += elapsed;
        }

        const auto average = total / iterations;

        const auto path = _output_root / "cpu" / std::format("{}x{}.png", output.width(), output.height());
        std::filesystem::create_directories(path.parent_path());
        imagegraph::image::save_to_file(output, path);

        return {output, min, max, average};
    }

    BenchmarkResult Benchmark::run_cpu_parallel() {
        const auto input = _input_node->image();
        auto output = image::Image(input.width(), input.height(), input.channels());

        std::uint64_t min = std::numeric_limits<std::uint64_t>::max();
        std::uint64_t max = 0;
        std::uint64_t total = 0;

        for (std::size_t i = 0; i < iterations; ++i) {
            const auto start = std::chrono::steady_clock::now();

            execute_cpu_parallel(input, output);

            const auto end = std::chrono::steady_clock::now();
            const auto elapsed = std::chrono::duration<std::uint64_t, std::nano>(end - start).count();

            min = std::min(min, elapsed);
            max = std::max(max, elapsed);
            total += elapsed;
        }

        const auto average = total / iterations;

        const auto path = _output_root / "cpu_par" / std::format("{}x{}.png", output.width(), output.height());
        std::filesystem::create_directories(path.parent_path());
        imagegraph::image::save_to_file(output, path);

        return {output, min, max, average};
    }
} // namespace imagegraph::benchmark
