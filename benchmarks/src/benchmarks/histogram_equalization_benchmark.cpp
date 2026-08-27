#include <benchmark/benchmarks/histogram_equalization_benchmark.h>

#include <imagegraph/compute/transfer.h>

#include <glm/gtc/type_ptr.hpp>

#include <limits>

namespace imagegraph::benchmark {
    HistogramEqualizationBenchmark::HistogramEqualizationBenchmark() : Benchmark("histogram_equalization") {
        const auto histogram_node = _graph.add_node(std::make_unique<nodes::HistogramEqualizationNode>());
        _graph.add_link(&_input_node->output_pins()[0], &histogram_node->input_pins()[0]);

        _histogram_node = dynamic_cast<nodes::HistogramEqualizationNode*>(histogram_node);
        _output_pin = &_histogram_node->output_pins()[0];
    }

    void HistogramEqualizationBenchmark::execute_cpu(const image::Image& input, image::Image& output) {
        const auto pixel_count = input.width() * input.height();
        constexpr auto weights = glm::vec3(0.2126, 0.7152, 0.0722);

        auto histogram = std::array<std::uint32_t, 256>();
        auto cdf = std::array<std::uint32_t, 256>();
        auto lut = std::array<float, 256>();

        for (std::size_t i = 0; i < pixel_count; ++i) {
            const auto index = i * 4;

            const auto pixel = glm::make_vec3(input.data() + index);
            const auto luminance = glm::clamp(glm::dot(pixel, weights), 0.0f, 1.0f);
            const auto bin = static_cast<std::uint32_t>(glm::round(luminance * 255.0f));

            ++histogram[bin];
        }

        auto sum = std::uint32_t{0};
        auto cdf_min = std::numeric_limits<std::uint32_t>::max();

        for (std::size_t i = 0; i < histogram.size(); ++i) {
            sum += histogram[i];
            cdf[i] = sum;

            if (histogram[i] > 0) {
                cdf_min = std::min(cdf_min, cdf[i]);
            }
        }

        for (std::size_t i = 0; i < lut.size(); ++i) {
            if (sum <= cdf_min) {
                lut[i] = static_cast<float>(i) / 255.0f;
            } else if (cdf[i] <= cdf_min) {
                lut[i] = 0.0f;
            } else {
                lut[i] = static_cast<float>(cdf[i] - cdf_min) / static_cast<float>(sum - cdf_min);
            }
        }


        for (std::size_t i = 0; i < pixel_count; ++i) {
            const auto index = i * 4;

            const auto pixel = glm::make_vec3(input.data() + index);
            const auto luminance = glm::clamp(glm::dot(pixel, weights), 0.0f, 1.0f);
            const auto bin = static_cast<uint32_t>(glm::round(luminance * 255.0f));
            const auto new_luminance = lut[bin];

            const auto rgb = luminance > 0.0f ? pixel * (new_luminance / luminance) : glm::vec3(new_luminance);

            output[index + 0] = rgb.r;
            output[index + 1] = rgb.g;
            output[index + 2] = rgb.b;
            output[index + 3] = input[index + 3];
        }
    }

    void HistogramEqualizationBenchmark::execute_cpu_parallel(const image::Image& input, image::Image& output) {
        const auto pixel_count = input.width() * input.height();
        constexpr auto weights = glm::vec3(0.2126, 0.7152, 0.0722);

        auto histogram = std::array<std::uint32_t, 256>();
        auto cdf = std::array<std::uint32_t, 256>();
        auto lut = std::array<float, 256>();

#pragma omp parallel
        {
            auto local_histogram = std::array<std::uint32_t, 256>();

#pragma omp for
            for (int i = 0; i < pixel_count; ++i) {
                const auto index = i * 4;

                const auto pixel = glm::make_vec3(input.data() + index);
                const auto luminance = glm::clamp(glm::dot(pixel, weights), 0.0f, 1.0f);
                const auto bin = static_cast<std::uint32_t>(glm::round(luminance * 255.0f));

                ++local_histogram[bin];
            }

#pragma omp critical
            {
                for (std::size_t i = 0; i < histogram.size(); ++i) {
                    histogram[i] += local_histogram[i];
                }
            }
        }

        auto sum = std::uint32_t{0};
        auto cdf_min = std::numeric_limits<std::uint32_t>::max();

        for (std::size_t i = 0; i < histogram.size(); ++i) {
            sum += histogram[i];
            cdf[i] = sum;

            if (histogram[i] > 0) {
                cdf_min = std::min(cdf_min, cdf[i]);
            }
        }

        for (std::size_t i = 0; i < lut.size(); ++i) {
            if (sum <= cdf_min) {
                lut[i] = static_cast<float>(i) / 255.0f;
            } else if (cdf[i] <= cdf_min) {
                lut[i] = 0.0f;
            } else {
                lut[i] = static_cast<float>(cdf[i] - cdf_min) / static_cast<float>(sum - cdf_min);
            }
        }

#pragma omp parallel for
        for (int i = 0; i < pixel_count; ++i) {
            const auto index = i * 4;

            const auto pixel = glm::make_vec3(input.data() + index);
            const auto luminance = glm::clamp(glm::dot(pixel, weights), 0.0f, 1.0f);
            const auto bin = static_cast<uint32_t>(glm::round(luminance * 255.0f));
            const auto new_luminance = lut[bin];

            const auto rgb = luminance > 0.0f ? pixel * (new_luminance / luminance) : glm::vec3(new_luminance);

            output[index + 0] = rgb.r;
            output[index + 1] = rgb.g;
            output[index + 2] = rgb.b;
            output[index + 3] = input[index + 3];
        }
    }
} // namespace imagegraph::benchmark
