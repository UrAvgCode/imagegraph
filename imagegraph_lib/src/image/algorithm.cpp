#include <image/algorithm.h>

#include <algorithm>

namespace imagegraph::image {
    MinMax min_max(const Image& image) {
        const auto begin = image.data();
        const auto end = begin + image.size();
        const auto [min, max] = std::minmax_element(begin, end);
        return {*min, *max};
    }

    void normalize(Image& image) {
        const auto [min, max] = min_max(image);

        const float range = max - min;
        if (range == 0.0f) {
            return;
        }

        for (std::size_t i = 0; i < image.size(); ++i) {
            image[i] = (image[i] - min) / range;
        }
    }
} // namespace imagegraph::image
