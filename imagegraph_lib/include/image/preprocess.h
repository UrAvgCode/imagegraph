#pragma once

#include <image/image.h>

#include <vector>

namespace imagegraph::image {
    std::vector<float> image_to_tensor(const Image&);
}
