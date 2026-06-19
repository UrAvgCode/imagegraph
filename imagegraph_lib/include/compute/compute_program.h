#pragma once

#include <glad/gl.h>

#include <glm/glm.hpp>

namespace imagegraph::compute {
    class ComputeProgram {
    public:
        ComputeProgram();
        ~ComputeProgram();

        ComputeProgram(const ComputeProgram&) = delete;
        ComputeProgram& operator=(const ComputeProgram&) = delete;

        ComputeProgram(ComputeProgram&&) noexcept;
        ComputeProgram& operator=(ComputeProgram&&) noexcept;

        void load(const char*);

        void dispatch(GLuint, GLuint) const;

        GLuint id() const;

        void set_uniform_float(GLint, GLfloat);
        void set_uniform_int(GLint, GLint);
        void set_uniform_uint(GLint, GLuint);

        void set_uniform_vec2(GLint, GLfloat, GLfloat);
        void set_uniform_vec2(GLint, const glm::vec2&);
        void set_uniform_vec3(GLint, GLfloat, GLfloat, GLfloat);
        void set_uniform_vec3(GLint, const glm::vec3&);
        void set_uniform_vec4(GLint, GLfloat, GLfloat, GLfloat, GLfloat);
        void set_uniform_vec4(GLint, const glm::vec4&);

        void set_uniform_ivec2(GLint, GLint, GLint);
        void set_uniform_ivec2(GLint, const glm::ivec2&);
        void set_uniform_ivec3(GLint, GLint, GLint, GLint);
        void set_uniform_ivec3(GLint, const glm::ivec3&);
        void set_uniform_ivec4(GLint, GLint, GLint, GLint, GLint);
        void set_uniform_ivec4(GLint, const glm::ivec4&);

        void set_uniform_uvec2(GLint, GLuint, GLuint);
        void set_uniform_uvec2(GLint, const glm::uvec2&);
        void set_uniform_uvec3(GLint, GLuint, GLuint, GLuint);
        void set_uniform_uvec3(GLint, const glm::uvec3&);
        void set_uniform_uvec4(GLint, GLuint, GLuint, GLuint, GLuint);
        void set_uniform_uvec4(GLint, const glm::uvec4&);

        void set_uniform_mat2(GLint, const glm::mat2&);
        void set_uniform_mat3(GLint, const glm::mat3&);
        void set_uniform_mat4(GLint, const glm::mat4&);

        void set_uniform_mat2x3(GLint, const glm::mat2x3&);
        void set_uniform_mat3x2(GLint, const glm::mat3x2&);
        void set_uniform_mat2x4(GLint, const glm::mat2x4&);
        void set_uniform_mat4x2(GLint, const glm::mat4x2&);
        void set_uniform_mat3x4(GLint, const glm::mat3x4&);
        void set_uniform_mat4x3(GLint, const glm::mat4x3&);

        GLint get_uniform_location(const char*) const;

    private:
        GLuint _id;
        glm::uvec3 _local_size;
    };
} // namespace imagegraph::compute
