#include <compute/mask.h>

#include <cassert>

namespace imagegraph::compute {
    Mask::Mask() : _id(0), _width(0), _height(0) {}

    Mask::~Mask() {
        if (_id) {
            glDeleteTextures(1, &_id);
        }
    }

    Mask::Mask(Mask&& other) noexcept : _id(other._id), _width(other._width), _height(other._height) {
        other._id = 0;
        other._width = 0;
        other._height = 0;
    }

    Mask& Mask::operator=(Mask&& other) noexcept {
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

    Mask::operator bool() const noexcept { return _id != 0; }

    void Mask::allocate(const GLsizei width, const GLsizei height) {
        if (_width == width && _height == height) {
            return;
        }

        if (_id) {
            glDeleteTextures(1, &_id);
        }

        _width = width;
        _height = height;

        glCreateTextures(GL_TEXTURE_2D, 1, &_id);

        glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTextureParameteri(_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(_id, GL_TEXTURE_WRAP_T, GL_REPEAT);

        constexpr GLint swizzle[] = {GL_RED, GL_RED, GL_RED, GL_ONE};
        glTextureParameteriv(_id, GL_TEXTURE_SWIZZLE_RGBA, swizzle);

        glTextureStorage2D(_id, 1, GL_R32F, _width, _height);
    }

    void Mask::allocate(const GLsizei width, const GLsizei height, const float* data) {
        allocate(width, height);
        glTextureSubImage2D(_id, 0, 0, 0, _width, _height, GL_RED, GL_FLOAT, data);
    }

    void Mask::bind(const GLuint unit) const {
        assert(_id != 0);
        glBindTextureUnit(unit, _id);
    }

    void Mask::bind_image(const GLuint unit, const GLenum access) const {
        assert(_id != 0);
        glBindImageTexture(unit, _id, 0, GL_FALSE, 0, access, GL_R32F);
    }

    GLuint Mask::id() const { return _id; }

    GLsizei Mask::width() const { return _width; }

    GLsizei Mask::height() const { return _height; }
} // namespace imagegraph::compute
