#pragma once

#include <compute/texture.h>
#include <image/image.h>

namespace imagegraph::compute {
    image::Image download_texture(const Texture* texture);
    Texture upload_image(const image::Image&);
    void upload_image(const image::Image&, Texture* texture);
} // namespace imagegraph::compute
