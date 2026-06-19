#pragma once

#include <imagegraph/image/image.h>

namespace imagegraph::image {
    struct MinMax {
        float min;
        float max;
    };

    MinMax min_max(const Image&);

    void normalize(Image&);
} // namespace imagegraph::image
