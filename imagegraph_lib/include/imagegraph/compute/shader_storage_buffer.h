#pragma once

#include <glad/gl.h>

namespace imagegraph::compute {
    class ShaderStorageBuffer {
    public:
        ShaderStorageBuffer();
        ~ShaderStorageBuffer();

        ShaderStorageBuffer(const ShaderStorageBuffer&) = delete;
        ShaderStorageBuffer& operator=(const ShaderStorageBuffer&) = delete;

        ShaderStorageBuffer(ShaderStorageBuffer&&) noexcept;
        ShaderStorageBuffer& operator=(ShaderStorageBuffer&&) noexcept;

        void allocate(GLsizeiptr);
        void clear() const;

        void bind_base(GLuint) const;

        GLuint id() const;
        GLsizeiptr size() const;

    private:
        GLuint _id;
        GLsizeiptr _size;
    };
} // namespace imagegraph::compute
