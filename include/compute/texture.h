#pragma once

#include <glad/gl.h>

namespace imagegraph::compute {
    class Texture {
    public:
        Texture();
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&&) noexcept;
        Texture& operator=(Texture&&) noexcept;

        explicit operator bool() const noexcept;

        void allocate(GLsizei width, GLsizei height);
        void allocate(GLsizei width, GLsizei height, const float*);

        void bind_image(GLuint unit, GLenum access) const;

        GLuint id() const;
        GLsizei width() const;
        GLsizei height() const;

    private:
        GLuint _id;
        GLsizei _width;
        GLsizei _height;
    };
} // namespace imagegraph::compute
