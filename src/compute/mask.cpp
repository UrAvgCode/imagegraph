#include <compute/mask.h>

namespace imagegraph::compute {
    Mask::Mask() : _texture(0), _width(0), _height(0) {}

    Mask::~Mask() {
        if (_texture) {
            glDeleteTextures(1, &_texture);
        }
    }

    Mask::Mask(Mask&& other) noexcept : _texture(other._texture), _width(other._width), _height(other._height) {
        other._texture = 0;
        other._width = 0;
        other._height = 0;
    }

    Mask& Mask::operator=(Mask&& other) noexcept {
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

    void Mask::allocate(const GLsizei width, const GLsizei height) { allocate(width, height, nullptr); }

    void Mask::allocate(const GLsizei width, const GLsizei height, const float* data) {
        if (_texture == 0) {
            glGenTextures(1, &_texture);
        }

        _width = width;
        _height = height;

        glBindTexture(GL_TEXTURE_2D, _texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, width, height, 0, GL_RED, GL_FLOAT, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        constexpr GLint swizzle[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
        glTextureParameteriv(_texture, GL_TEXTURE_SWIZZLE_RGBA, swizzle);
    }

    void Mask::bind(const GLuint unit) const {
        if (_texture) {
            glActiveTexture(GL_TEXTURE0 + unit);
            glBindTexture(GL_TEXTURE_2D, _texture);
        }
    }

    void Mask::unbind(const GLuint unit) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void Mask::bind_image(const GLuint unit, const GLenum access) const {
        glBindImageTexture(unit, _texture, 0, GL_FALSE, 0, access, GL_R32F);
    }

    void Mask::unbind_image(const GLuint unit) { glBindImageTexture(unit, 0, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F); }

    GLuint Mask::id() const { return _texture; }

    GLsizei Mask::width() const { return _width; }

    GLsizei Mask::height() const { return _height; }
} // namespace imagegraph::compute
