#include <benchmark/benchmarks/blur_benchmark.h>

#include <glm/gtc/type_ptr.hpp>

#include <algorithm>
#include <vector>

namespace {
    std::vector<float> gaussian_kernel(const std::size_t radius) {
        const float sigma = static_cast<float>(radius) / 3.0f;

        auto kernel = std::vector<float>(radius + 1);
        for (std::size_t i = 0; i < kernel.size(); ++i) {
            kernel[i] = std::exp(-static_cast<float>(i * i) / (2.0f * sigma * sigma));
        }

        return kernel;
    }
} // namespace

namespace imagegraph::benchmark {
    constexpr auto radius = 100;

    BlurBenchmark::BlurBenchmark() : Benchmark("blur") {
        const auto blur_node = _graph.add_node(std::make_unique<nodes::GaussianBlurNode>(radius, radius));
        _graph.add_link(&_input_node->output_pins()[0], &blur_node->input_pins()[0]);

        _blur_node = dynamic_cast<nodes::GaussianBlurNode*>(blur_node);
        _output_pin = &_blur_node->output_pins()[0];
    }

    void BlurBenchmark::execute_cpu(const image::Image& input, image::Image& output) {
        const int width = input.width();
        const int height = input.height();

        const auto horizontal_kernel = gaussian_kernel(radius);
        auto temp_image = image::Image(width, height, input.channels());

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                auto color = glm::vec4(0.0f);
                float weight_sum = 0.0f;

                for (int j = -radius; j <= radius; ++j) {
                    const int x_coord = std::clamp(x + j, 0, width - 1);
                    const int index = (y * width + x_coord) * 4;

                    const float weight = horizontal_kernel[std::abs(j)];
                    color += glm::make_vec4(input.data() + index) * weight;
                    weight_sum += weight;
                }

                const auto pixel = color / weight_sum;
                const int index = (y * width + x) * 4;

                temp_image[index + 0] = pixel.r;
                temp_image[index + 1] = pixel.g;
                temp_image[index + 2] = pixel.b;
                temp_image[index + 3] = pixel.a;
            }
        }

        const auto vertical_kernel = gaussian_kernel(radius);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                auto color = glm::vec4(0.0f);
                float weight_sum = 0.0f;

                for (int j = -radius; j <= radius; ++j) {
                    const int y_coord = std::clamp(y + j, 0, height - 1);
                    const int index = (y_coord * width + x) * 4;

                    const float weight = vertical_kernel[std::abs(j)];
                    color += glm::make_vec4(temp_image.data() + index) * weight;
                    weight_sum += weight;
                }

                const auto pixel = color / weight_sum;
                const int index = (y * width + x) * 4;

                output[index + 0] = pixel.r;
                output[index + 1] = pixel.g;
                output[index + 2] = pixel.b;
                output[index + 3] = pixel.a;
            }
        }
    }

    void BlurBenchmark::execute_cpu_parallel(const image::Image& input, image::Image& output) {
        const int width = input.width();
        const int height = input.height();

        const auto horizontal_kernel = gaussian_kernel(radius);
        auto temp_image = image::Image(width, height, input.channels());

#pragma omp parallel for schedule(static)
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                auto color = glm::vec4(0.0f);
                float weight_sum = 0.0f;

                for (int j = -radius; j <= radius; ++j) {
                    const int x_coord = std::clamp(x + j, 0, width - 1);
                    const int index = (y * width + x_coord) * 4;

                    const float weight = horizontal_kernel[std::abs(j)];
                    color += glm::make_vec4(input.data() + index) * weight;
                    weight_sum += weight;
                }

                const auto pixel = color / weight_sum;
                const int index = (y * width + x) * 4;

                temp_image[index + 0] = pixel.r;
                temp_image[index + 1] = pixel.g;
                temp_image[index + 2] = pixel.b;
                temp_image[index + 3] = pixel.a;
            }
        }

        const auto vertical_kernel = gaussian_kernel(radius);

#pragma omp parallel for schedule(static)
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                auto color = glm::vec4(0.0f);
                float weight_sum = 0.0f;

                for (int j = -radius; j <= radius; ++j) {
                    const int y_coord = std::clamp(y + j, 0, height - 1);
                    const int index = (y_coord * width + x) * 4;

                    const float weight = vertical_kernel[std::abs(j)];
                    color += glm::make_vec4(temp_image.data() + index) * weight;
                    weight_sum += weight;
                }

                const auto pixel = color / weight_sum;
                const int index = (y * width + x) * 4;

                output[index + 0] = pixel.r;
                output[index + 1] = pixel.g;
                output[index + 2] = pixel.b;
                output[index + 3] = pixel.a;
            }
        }
    }
} // namespace imagegraph::benchmark
