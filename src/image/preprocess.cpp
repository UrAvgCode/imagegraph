#include <image/preprocess.h>

#include <array>
#include <cassert>

namespace imagegraph::image {
    std::vector<float> image_to_tensor(const Image& image) {
        assert(image.channels() >= 3);

        constexpr auto mean = std::array{0.485f, 0.456f, 0.406f};
        constexpr auto std_dev = std::array{0.229f, 0.224f, 0.225f};

        const auto pixel_count = image.width() * image.height();
        auto tensor_values = std::vector<float>(pixel_count * 3);

        for (std::size_t i = 0; i < pixel_count; ++i) {
            const auto pixel_index = i * image.channels();

            const auto red = image[pixel_index + 0];
            const auto green = image[pixel_index + 1];
            const auto blue = image[pixel_index + 2];

            tensor_values[0 * pixel_count + i] = (red - mean[0]) / std_dev[0];
            tensor_values[1 * pixel_count + i] = (green - mean[1]) / std_dev[1];
            tensor_values[2 * pixel_count + i] = (blue - mean[2]) / std_dev[2];
        }

        return tensor_values;
    }
} // namespace imagegraph::image
