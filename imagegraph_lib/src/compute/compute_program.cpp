#include <compute/compute_program.h>

#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <cassert>
#include <cstdio>

namespace {
    GLuint create_program(const char* source) {
        assert(source != nullptr);

        const auto program = glCreateShaderProgramv(GL_COMPUTE_SHADER, 1, &source);

        GLint success = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            constexpr int buffer_size = 2048;
            auto buffer = std::array<GLchar, buffer_size>();

            glGetProgramInfoLog(program, buffer_size, nullptr, buffer.data());
            std::fprintf(stderr, "program linking failed:\n%s\n", buffer.data());

            glDeleteProgram(program);
            return 0;
        }

        return program;
    }
} // namespace

namespace imagegraph::compute {
    ComputeProgram::ComputeProgram() : _id(0), _local_size(1, 1, 1) {}

    ComputeProgram::~ComputeProgram() {
        if (_id != 0) {
            glDeleteProgram(_id);
        }
    }

    ComputeProgram::ComputeProgram(ComputeProgram&& other) noexcept : _id(other._id), _local_size(other._local_size) {
        other._id = 0;
        other._local_size = {1, 1, 1};
    }

    ComputeProgram& ComputeProgram::operator=(ComputeProgram&& other) noexcept {
        if (this != &other) {
            if (_id != 0) {
                glDeleteProgram(_id);
            }
            _id = other._id;
            _local_size = other._local_size;
            other._id = 0;
            other._local_size = {1, 1, 1};
        }
        return *this;
    }

    void ComputeProgram::load(const char* source) {
        const auto loaded_program = create_program(source);

        if (_id != 0) {
            glDeleteProgram(_id);
        }
        _id = loaded_program;

        auto local_size = std::array<GLint, 3>{1, 1, 1};
        glGetProgramiv(_id, GL_COMPUTE_WORK_GROUP_SIZE, local_size.data());
        _local_size = {local_size[0], local_size[1], local_size[2]};
    }

    void ComputeProgram::dispatch(const GLuint width, const GLuint height) const {
        assert(width > 0 && height > 0);
        assert(_id != 0);

        const GLuint groups_x = (width + _local_size.x - 1) / _local_size.x;
        const GLuint groups_y = (height + _local_size.y - 1) / _local_size.y;

        glUseProgram(_id);
        glDispatchCompute(groups_x, groups_y, 1);
        glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    GLuint ComputeProgram::id() const { return _id; }

    void ComputeProgram::set_uniform_float(const GLint location, const GLfloat value) {
        glProgramUniform1f(_id, location, value);
    }

    void ComputeProgram::set_uniform_int(const GLint location, const GLint value) {
        glProgramUniform1i(_id, location, value);
    }

    void ComputeProgram::set_uniform_uint(const GLint location, const GLuint value) {
        glProgramUniform1ui(_id, location, value);
    }

    void ComputeProgram::set_uniform_vec2(const GLint location, const GLfloat x, const GLfloat y) {
        glProgramUniform2f(_id, location, x, y);
    }

    void ComputeProgram::set_uniform_vec2(const GLint location, const glm::vec2& value) {
        glProgramUniform2fv(_id, location, 1, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_vec3(const GLint location, const GLfloat x, const GLfloat y, const GLfloat z) {
        glProgramUniform3f(_id, location, x, y, z);
    }

    void ComputeProgram::set_uniform_vec3(const GLint location, const glm::vec3& value) {
        glProgramUniform3fv(_id, location, 1, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_vec4(const GLint location, const GLfloat x, const GLfloat y, const GLfloat z,
                                          const GLfloat w) {
        glProgramUniform4f(_id, location, x, y, z, w);
    }

    void ComputeProgram::set_uniform_vec4(const GLint location, const glm::vec4& value) {
        glProgramUniform4fv(_id, location, 1, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_ivec2(const GLint location, const GLint x, const GLint y) {
        glProgramUniform2i(_id, location, x, y);
    }

    void ComputeProgram::set_uniform_ivec2(const GLint location, const glm::ivec2& value) {
        glProgramUniform2iv(_id, location, 1, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_ivec3(const GLint location, const GLint x, const GLint y, const GLint z) {
        glProgramUniform3i(_id, location, x, y, z);
    }

    void ComputeProgram::set_uniform_ivec3(const GLint location, const glm::ivec3& value) {
        glProgramUniform3iv(_id, location, 1, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_ivec4(const GLint location, const GLint x, const GLint y, const GLint z,
                                           const GLint w) {
        glProgramUniform4i(_id, location, x, y, z, w);
    }

    void ComputeProgram::set_uniform_ivec4(const GLint location, const glm::ivec4& value) {
        glProgramUniform4iv(_id, location, 1, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_uvec2(const GLint location, const GLuint x, const GLuint y) {
        glProgramUniform2ui(_id, location, x, y);
    }

    void ComputeProgram::set_uniform_uvec2(const GLint location, const glm::uvec2& value) {
        glProgramUniform2uiv(_id, location, 1, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_uvec3(const GLint location, const GLuint x, const GLuint y, const GLuint z) {
        glProgramUniform3ui(_id, location, x, y, z);
    }

    void ComputeProgram::set_uniform_uvec3(const GLint location, const glm::uvec3& value) {
        glProgramUniform3uiv(_id, location, 1, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_uvec4(const GLint location, const GLuint x, const GLuint y, const GLuint z,
                                           const GLuint w) {
        glProgramUniform4ui(_id, location, x, y, z, w);
    }

    void ComputeProgram::set_uniform_uvec4(const GLint location, const glm::uvec4& value) {
        glProgramUniform4uiv(_id, location, 1, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_mat2(const GLint location, const glm::mat2& value) {
        glProgramUniformMatrix2fv(_id, location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_mat3(const GLint location, const glm::mat3& value) {
        glProgramUniformMatrix3fv(_id, location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_mat4(const GLint location, const glm::mat4& value) {
        glProgramUniformMatrix4fv(_id, location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_mat2x3(const GLint location, const glm::mat2x3& value) {
        glProgramUniformMatrix2x3fv(_id, location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_mat3x2(const GLint location, const glm::mat3x2& value) {
        glProgramUniformMatrix3x2fv(_id, location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_mat2x4(const GLint location, const glm::mat2x4& value) {
        glProgramUniformMatrix2x4fv(_id, location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_mat4x2(const GLint location, const glm::mat4x2& value) {
        glProgramUniformMatrix4x2fv(_id, location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_mat3x4(const GLint location, const glm::mat3x4& value) {
        glProgramUniformMatrix3x4fv(_id, location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_mat4x3(const GLint location, const glm::mat4x3& value) {
        glProgramUniformMatrix4x3fv(_id, location, 1, GL_FALSE, glm::value_ptr(value));
    }

    GLint ComputeProgram::get_uniform_location(const char* name) const {
        const auto location = glGetUniformLocation(_id, name);
        if (location == -1) {
            std::fprintf(stderr, "warning: uniform '%s' not found in program %u\n", name, _id);
        }
        return location;
    }
} // namespace imagegraph::compute
