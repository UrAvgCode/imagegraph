#include <benchmark/benchmarks/depth_model_benchmark.h>

#include <imagegraph/image/io.h>
#include <imagegraph/inference/depth_model.h>

#include <cstdio>
#include <format>
#include <fstream>

namespace {
    constexpr std::size_t iterations = 10;
}

namespace imagegraph::benchmark {
    DepthModelBenchmark::DepthModelBenchmark() : _output_root("outputs/depth_model") {
        std::filesystem::create_directories(_output_root);
    }

    void DepthModelBenchmark::run() {
        auto file = std::ofstream(_output_root / "results.csv");
        file << "size,gpu,cpu\n";

        std::printf("running depth model benchmark\n");
        for (int size = 56; size <= 1064; size += 56) {
            std::printf("benchmarking %dx%d...\n", size, size);
            _input_node.set_size(size, size);

            const auto gpu_result = run_model(inference::Device::Cuda);
            const auto cpu_result = run_model(inference::Device::Cpu);

            file << size << ',' << gpu_result << ',' << cpu_result << '\n';
        }
    }

    std::uint64_t DepthModelBenchmark::run_model(const inference::Device device) {
        auto depth_model = inference::DepthModel(device);

        const auto image = _input_node.image();
        const auto size = std::array{image.width(), image.height()};

        auto output = image::Image();

        std::uint64_t total = 0;
        for (std::size_t i = 0; i < iterations; ++i) {
            const auto start = std::chrono::steady_clock::now();

            output = depth_model.run(image, size);

            const auto end = std::chrono::steady_clock::now();
            const auto elapsed = std::chrono::duration<std::uint64_t, std::nano>(end - start).count();

            total += elapsed;
        }

        const auto average = total / iterations;

        const auto subfolder = device == inference::Device::Cpu ? "cpu" : "gpu";
        const auto path = _output_root / subfolder / std::format("{}x{}.png", output.width(), output.height());
        std::filesystem::create_directories(path.parent_path());
        imagegraph::image::save_to_file(output, path);

        return average;
    }
} // namespace imagegraph::benchmark
