#include <benchmark/benchmarks/segment_model_benchmark.h>

#include <imagegraph/image/io.h>
#include <imagegraph/inference/segment_model.h>

#include <cstdio>
#include <format>
#include <fstream>

namespace {
    constexpr std::size_t iterations = 10;
}

namespace imagegraph::benchmark {
    SegmentModelBenchmark::SegmentModelBenchmark() : _output_root("outputs/segment_model") {
        std::filesystem::create_directories(_output_root);
    }

    void SegmentModelBenchmark::run() {
        auto file = std::ofstream(_output_root / "results.csv");
        file << "gpu_encoder,gpu_decoder,cpu_encoder,cpu_decoder\n";

        std::printf("running segment model benchmark\n");

        _input_node.set_size(1024, 1024);
        const auto gpu_result = run_model(inference::Device::Cuda);
        const auto cpu_result = run_model(inference::Device::Cpu);

        file << gpu_result[0] << ',' << gpu_result[1] << ',' << cpu_result[0] << ',' << cpu_result[1] << '\n';
    }

    std::array<std::uint64_t, 2> SegmentModelBenchmark::run_model(const inference::Device device) {
        auto segment_model = inference::SegmentModel(device);

        const auto image = _input_node.image();
        auto decoder_inputs = inference::DecoderInputs();

        std::uint64_t total_encoder = 0;
        for (std::size_t i = 0; i < iterations; ++i) {
            const auto start = std::chrono::steady_clock::now();

            decoder_inputs = segment_model.encode(image);

            const auto end = std::chrono::steady_clock::now();
            const auto elapsed = std::chrono::duration<std::uint64_t, std::nano>(end - start).count();

            total_encoder += elapsed;
        }

        const auto average_encoder = total_encoder / iterations;

        auto output = image::Image();

        std::uint64_t total_decoder = 0;
        for (std::size_t i = 0; i < iterations; ++i) {
            const auto start = std::chrono::steady_clock::now();

            output = segment_model.decode(decoder_inputs, {0.5, 0.5});

            const auto end = std::chrono::steady_clock::now();
            const auto elapsed = std::chrono::duration<std::uint64_t, std::nano>(end - start).count();

            total_decoder += elapsed;
        }

        const auto average_decoder = total_decoder / iterations;

        const auto subfolder = device == inference::Device::Cpu ? "cpu" : "gpu";
        const auto path = _output_root / subfolder / std::format("{}x{}.png", output.width(), output.height());
        std::filesystem::create_directories(path.parent_path());
        imagegraph::image::save_to_file(output, path);

        return {average_encoder, average_decoder};
    }
} // namespace imagegraph::benchmark
