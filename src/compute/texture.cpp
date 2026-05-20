#include <compute/texture.h>

#include <cassert>

namespace imagegraph::compute {
    Texture::Texture() : _id(0), _width(0), _height(0) {}

    Texture::~Texture() {
        if (_id) {
            glDeleteTextures(1, &_id);
        }
    }

    Texture::Texture(Texture&& other) noexcept : _id(other._id), _width(other._width), _height(other._height) {
        other._id = 0;
        other._width = 0;
        other._height = 0;
    }

    Texture& Texture::operator=(Texture&& other) noexcept {
        if (this != &other) {
            if (_id) {
                glDeleteTextures(1, &_id);
            }
            _id = other._id;
            _width = other._width;
            _height = other._height;
            other._id = 0;
            other._width = 0;
            other._height = 0;
        }
        return *this;
    }

    Texture::operator bool() const noexcept { return _id != 0; }

    void Texture::allocate(const GLsizei width, const GLsizei height) {
        if (_id == 0) {
            glCreateTextures(GL_TEXTURE_2D, 1, &_id);

            glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTextureParameteri(_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            glTextureParameteri(_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTextureParameteri(_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        }

        if (_width != width || _height != height) {
            _width = width;
            _height = height;
            glTextureStorage2D(_id, 1, GL_RGBA32F, _width, _height);
        }
    }

    void Texture::allocate(const GLsizei width, const GLsizei height, const float* data) {
        allocate(width, height);
        glTextureSubImage2D(_id, 0, 0, 0, _width, _height, GL_RGBA, GL_FLOAT, data);
    }

    void Texture::bind_image(const GLuint unit, const GLenum access) const {
        assert(_id != 0);
        glBindImageTexture(unit, _id, 0, GL_FALSE, 0, access, GL_RGBA32F);
    }

    GLuint Texture::id() const { return _id; }

    GLsizei Texture::width() const { return _width; }

    GLsizei Texture::height() const { return _height; }
} // namespace imagegraph::compute
