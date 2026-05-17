#include <image/algorithm.h>

#include <algorithm>
#include <limits>

namespace imagegraph::image {
    MinMax min_max(const Image& image) {
        float min_value = std::numeric_limits<float>::max();
        float max_value = std::numeric_limits<float>::lowest();

        for (std::size_t i = 0; i < image.size(); ++i) {
            float value = image[i];
            min_value = std::min(min_value, value);
            max_value = std::max(max_value, value);
        }

        return {min_value, max_value};
    }

    void normalize(Image& image) {
        const auto [min, max] = min_max(image);
        const float range = max - min;

        for (std::size_t i = 0; i < image.size(); ++i) {
            image[i] = (image[i] - min) / range;
        }
    }
} // namespace imagegraph::image
