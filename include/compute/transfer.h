#pragma once

#include <compute/mask.h>
#include <compute/texture.h>
#include <image/image.h>

namespace imagegraph::compute {
    image::Image download_texture(const Texture* texture);
    void upload_image(const image::Image&, Texture* texture);

    image::Image download_mask(const Mask* mask);
    void upload_mask(const image::Image&, Mask* mask);
} // namespace imagegraph::compute
