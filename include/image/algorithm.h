#pragma once

#include <image/image.h>

namespace imagegraph::image {
    struct MinMax {
        float min;
        float max;
    };

    MinMax min_max(const Image&);

    void normalize(Image&);
} // namespace imagegraph::image
