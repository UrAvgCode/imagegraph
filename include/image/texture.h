#pragma once

#include <glad/glad.h>

namespace imagegraph::image {
    class Texture {
    public:
        Texture();
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&&) noexcept;
        Texture& operator=(Texture&&) noexcept;

        void allocate(GLsizei width, GLsizei height);
        void allocate(GLsizei width, GLsizei height, const float*);

        void bind(GLuint unit) const;
        static void unbind(GLuint unit);

        GLuint id() const;
        GLsizei width() const;
        GLsizei height() const;

    private:
        GLuint _texture;
        GLsizei _width;
        GLsizei _height;
    };
} // namespace imagegraph::image
