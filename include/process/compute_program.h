#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

namespace imagegraph::process {
    class ComputeProgram {
    public:
        ComputeProgram();
        ~ComputeProgram();

        ComputeProgram(const ComputeProgram&) = delete;
        ComputeProgram& operator=(const ComputeProgram&) = delete;

        ComputeProgram(ComputeProgram&&) noexcept;
        ComputeProgram& operator=(ComputeProgram&&) noexcept;

        void load(const char*);

        void bind() const;
        static void unbind();

        void dispatch(GLuint, GLuint) const;

        GLuint id() const;

        void set_uniform_float(const char*, GLfloat) const;
        void set_uniform_int(const char*, GLint) const;
        void set_uniform_uint(const char*, GLuint) const;

        void set_uniform_vec2(const char*, GLfloat, GLfloat) const;
        void set_uniform_vec2(const char*, const glm::vec2&) const;
        void set_uniform_vec3(const char*, GLfloat, GLfloat, GLfloat) const;
        void set_uniform_vec3(const char*, const glm::vec3&) const;
        void set_uniform_vec4(const char*, GLfloat, GLfloat, GLfloat, GLfloat) const;
        void set_uniform_vec4(const char*, const glm::vec4&) const;

        void set_uniform_ivec2(const char*, GLint, GLint) const;
        void set_uniform_ivec2(const char*, const glm::ivec2&) const;
        void set_uniform_ivec3(const char*, GLint, GLint, GLint) const;
        void set_uniform_ivec3(const char*, const glm::ivec3&) const;
        void set_uniform_ivec4(const char*, GLint, GLint, GLint, GLint) const;
        void set_uniform_ivec4(const char*, const glm::ivec4&) const;

        void set_uniform_uvec2(const char*, GLuint, GLuint) const;
        void set_uniform_uvec2(const char*, const glm::uvec2&) const;
        void set_uniform_uvec3(const char*, GLuint, GLuint, GLuint) const;
        void set_uniform_uvec3(const char*, const glm::uvec3&) const;
        void set_uniform_uvec4(const char*, GLuint, GLuint, GLuint, GLuint) const;
        void set_uniform_uvec4(const char*, const glm::uvec4&) const;

        void set_uniform_mat2(const char*, const glm::mat2&) const;
        void set_uniform_mat3(const char*, const glm::mat3&) const;
        void set_uniform_mat4(const char*, const glm::mat4&) const;

        void set_uniform_mat2x3(const char*, const glm::mat2x3&) const;
        void set_uniform_mat3x2(const char*, const glm::mat3x2&) const;
        void set_uniform_mat2x4(const char*, const glm::mat2x4&) const;
        void set_uniform_mat4x2(const char*, const glm::mat4x2&) const;
        void set_uniform_mat3x4(const char*, const glm::mat3x4&) const;
        void set_uniform_mat4x3(const char*, const glm::mat4x3&) const;

        static void set_uniform_float(GLint, GLfloat);
        static void set_uniform_int(GLint, GLint);
        static void set_uniform_uint(GLint, GLuint);

        static void set_uniform_vec2(GLint, GLfloat, GLfloat);
        static void set_uniform_vec2(GLint, const glm::vec2&);
        static void set_uniform_vec3(GLint, GLfloat, GLfloat, GLfloat);
        static void set_uniform_vec3(GLint, const glm::vec3&);
        static void set_uniform_vec4(GLint, GLfloat, GLfloat, GLfloat, GLfloat);
        static void set_uniform_vec4(GLint, const glm::vec4&);

        static void set_uniform_ivec2(GLint, GLint, GLint);
        static void set_uniform_ivec2(GLint, const glm::ivec2&);
        static void set_uniform_ivec3(GLint, GLint, GLint, GLint);
        static void set_uniform_ivec3(GLint, const glm::ivec3&);
        static void set_uniform_ivec4(GLint, GLint, GLint, GLint, GLint);
        static void set_uniform_ivec4(GLint, const glm::ivec4&);

        static void set_uniform_uvec2(GLint, GLuint, GLuint);
        static void set_uniform_uvec2(GLint, const glm::uvec2&);
        static void set_uniform_uvec3(GLint, GLuint, GLuint, GLuint);
        static void set_uniform_uvec3(GLint, const glm::uvec3&);
        static void set_uniform_uvec4(GLint, GLuint, GLuint, GLuint, GLuint);
        static void set_uniform_uvec4(GLint, const glm::uvec4&);

        static void set_uniform_mat2(GLint, const glm::mat2&);
        static void set_uniform_mat3(GLint, const glm::mat3&);
        static void set_uniform_mat4(GLint, const glm::mat4&);

        static void set_uniform_mat2x3(GLint, const glm::mat2x3&);
        static void set_uniform_mat3x2(GLint, const glm::mat3x2&);
        static void set_uniform_mat2x4(GLint, const glm::mat2x4&);
        static void set_uniform_mat4x2(GLint, const glm::mat4x2&);
        static void set_uniform_mat3x4(GLint, const glm::mat3x4&);
        static void set_uniform_mat4x3(GLint, const glm::mat4x3&);

        GLint get_uniform_location(const char*) const;

    private:
        GLuint _program;
        glm::uvec3 _local_size;
    };
} // namespace imagegraph::process
