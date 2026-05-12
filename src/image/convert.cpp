#include <image/convert.h>

#include <cassert>

namespace imagegraph::image {
    Image read_texture(const Texture& texture) {
        auto image = Image(texture.width(), texture.height(), 4);
        glGetTextureImage(texture.id(), 0, GL_RGBA, GL_FLOAT, static_cast<GLsizei>(image.size_in_bytes()),
                          image.data());
        return image;
    }

    Texture create_texture(const Image& image) {
        assert(image.channels() == 4);
        auto texture = Texture();
        texture.allocate(image.width(), image.height(), image.data());
        return texture;
    }

    void update_texture(Texture* texture, const Image& image) {
        assert(image.channels() == 4);
        texture->allocate(image.width(), image.height(), image.data());
    }
} // namespace imagegraph::image
