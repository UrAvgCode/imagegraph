#include <image/texture.h>

namespace imagegraph::image {
    Texture::Texture() : _texture(0), _width(0), _height(0) {}

    Texture::~Texture() {
        if (_texture) {
            glDeleteTextures(1, &_texture);
        }
    }

    Texture::Texture(Texture&& other) noexcept :
        _texture(other._texture), _width(other._width), _height(other._height) {
        other._texture = 0;
        other._width = 0;
        other._height = 0;
    }

    Texture& Texture::operator=(Texture&& other) noexcept {
        if (this != &other) {
            if (_texture) {
                glDeleteTextures(1, &_texture);
            }
            _texture = other._texture;
            _width = other._width;
            _height = other._height;
            other._texture = 0;
            other._width = 0;
            other._height = 0;
        }
        return *this;
    }

    void Texture::allocate(const GLsizei width, const GLsizei height) { allocate(width, height, nullptr); }

    void Texture::allocate(const GLsizei width, const GLsizei height, const float* data) {
        if (_texture == 0) {
            glGenTextures(1, &_texture);
        }

        _width = width;
        _height = height;

        glBindTexture(GL_TEXTURE_2D, _texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    void Texture::bind(const GLuint unit) const {
        if (_texture) {
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, _texture);
        }
    }

    void Texture::unbind(const GLuint unit) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Texture::bind_image(const GLuint unit, const GLenum access) const {
        glBindImageTexture(unit, _texture, 0, GL_FALSE, 0, access, GL_RGBA32F);
    }

    void Texture::unbind_image(const GLuint unit) {
        glBindImageTexture(unit, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    }

    GLuint Texture::id() const { return _texture; }

    GLsizei Texture::width() const { return _width; }

    GLsizei Texture::height() const { return _height; }
} // namespace imagegraph::image
