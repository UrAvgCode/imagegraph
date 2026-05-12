#pragma once

#include <image/image.h>
#include <image/texture.h>

namespace imagegraph::image {
    Image read_texture(const Texture&);
    Texture create_texture(const Image&);
    void update_texture(Texture* texture, const Image&);
} // namespace imagegraph::image
