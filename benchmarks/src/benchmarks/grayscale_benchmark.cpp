#include <benchmark/benchmarks/grayscale_benchmark.h>

#include <imagegraph/compute/transfer.h>

#include <glm/gtc/type_ptr.hpp>

namespace imagegraph::benchmark {
    GrayscaleBenchmark::GrayscaleBenchmark() : Benchmark("grayscale") {
        const auto color_node = _graph.add_node(std::make_unique<nodes::ColorIntensityNode>(0.0f, 0.0f));
        _graph.add_link(&_input_node->output_pins()[0], &color_node->input_pins()[0]);

        _grayscale_node = dynamic_cast<nodes::ColorIntensityNode*>(color_node);
        _output_pin = &_grayscale_node->output_pins()[0];
    }

    void GrayscaleBenchmark::execute_cpu(const image::Image& input, image::Image& output) {
        const auto pixel_count = input.width() * input.height();
        constexpr auto weights = glm::vec3(0.2126, 0.7152, 0.0722);

        for (int i = 0; i < pixel_count; ++i) {
            const auto index = i * 4;

            const auto pixel = glm::make_vec3(input.data() + index);
            const auto luminance = glm::dot(pixel, weights);

            output[index + 0] = luminance;
            output[index + 1] = luminance;
            output[index + 2] = luminance;
            output[index + 3] = 1.0f;
        }
    }

    void GrayscaleBenchmark::execute_cpu_parallel(const image::Image& input, image::Image& output) {
        const auto pixel_count = input.width() * input.height();
        constexpr auto weights = glm::vec3(0.2126, 0.7152, 0.0722);

#pragma omp parallel for
        for (int i = 0; i < pixel_count; ++i) {
            const auto index = i * 4;

            const auto pixel = glm::make_vec3(input.data() + index);
            const auto luminance = glm::dot(pixel, weights);

            output[index + 0] = luminance;
            output[index + 1] = luminance;
            output[index + 2] = luminance;
            output[index + 3] = 1.0f;
        }
    }
} // namespace imagegraph::benchmark
