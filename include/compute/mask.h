#pragma once

#include <glad/gl.h>

namespace imagegraph::compute {
    class Mask {
    public:
        Mask();
        ~Mask();

        Mask(const Mask&) = delete;
        Mask& operator=(const Mask&) = delete;

        Mask(Mask&&) noexcept;
        Mask& operator=(Mask&&) noexcept;

        explicit operator bool() const noexcept;

        void allocate(GLsizei width, GLsizei height);
        void allocate(GLsizei width, GLsizei height, const float*);

        void bind(GLuint unit) const;
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
