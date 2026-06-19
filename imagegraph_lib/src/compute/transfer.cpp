#include <compute/transfer.h>

#include <cassert>

namespace imagegraph::compute {
    image::Image download_texture(const Texture* texture) {
        auto image = image::Image(texture->width(), texture->height(), 4);
        glGetTextureImage(texture->id(), 0, GL_RGBA, GL_FLOAT, static_cast<GLsizei>(image.size_in_bytes()),
                          image.data());
        return image;
    }

    void upload_image(const image::Image& image, Texture* texture) {
        assert(image.channels() == 4);
        texture->allocate(image.width(), image.height(), image.data());
    }

    image::Image download_mask(const Mask* mask) {
        auto image = image::Image(mask->width(), mask->height(), 1);
        glGetTextureImage(mask->id(), 0, GL_RED, GL_FLOAT, static_cast<GLsizei>(image.size_in_bytes()), image.data());
        return image;
    }

    void upload_mask(const image::Image& image, Mask* mask) {
        assert(image.channels() == 1);
        mask->allocate(image.width(), image.height(), image.data());
    }
} // namespace imagegraph::compute
