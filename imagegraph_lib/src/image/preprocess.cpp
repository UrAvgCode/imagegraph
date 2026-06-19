#include <image/preprocess.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/vec3.hpp>

#include <cassert>

namespace imagegraph::image {
    std::vector<float> image_to_tensor(const Image& image) {
        assert(image.channels() == 3 || image.channels() == 4);

        constexpr auto mean = glm::vec3(0.485f, 0.456f, 0.406f);
        constexpr auto std_dev = glm::vec3(0.229f, 0.224f, 0.225f);

        const auto pixel_count = image.width() * image.height();
        auto tensor_values = std::vector<float>(pixel_count * 3);

        for (std::size_t i = 0; i < pixel_count; ++i) {
            const auto index = i * image.channels();

            auto pixel = glm::make_vec3(image.data() + index);
            pixel = (pixel - mean) / std_dev;

            tensor_values[0 * pixel_count + i] = pixel.r;
            tensor_values[1 * pixel_count + i] = pixel.g;
            tensor_values[2 * pixel_count + i] = pixel.b;
        }

        return tensor_values;
    }
} // namespace imagegraph::image
