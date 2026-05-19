#pragma once

#include <glad/glad.h>

namespace imagegraph::compute {
    class Mask {
    public:
        Mask();
        ~Mask();

        Mask(const Mask&) = delete;
        Mask& operator=(const Mask&) = delete;

        Mask(Mask&&) noexcept;
        Mask& operator=(Mask&&) noexcept;

        void allocate(GLsizei width, GLsizei height);
        void allocate(GLsizei width, GLsizei height, const float*);

        void bind(GLuint unit) const;
        static void unbind(GLuint unit);

        void bind_image(GLuint unit, GLenum access) const;
        static void unbind_image(GLuint unit);

        GLuint id() const;
        GLsizei width() const;
        GLsizei height() const;

    private:
        GLuint _texture;
        GLsizei _width;
        GLsizei _height;
    };
} // namespace imagegraph::compute
