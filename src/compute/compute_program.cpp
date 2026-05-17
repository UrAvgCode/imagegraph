#include <compute/compute_program.h>

#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <cstdio>

namespace {
    GLuint compile_shader(const char* source) {
        const auto shader = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            constexpr int buffer_size = 2048;
            auto buffer = std::array<GLchar, buffer_size>();

            glGetShaderInfoLog(shader, buffer_size, nullptr, buffer.data());
            std::fprintf(stderr, "shader compilation failed:\n%s\n", buffer.data());

            glDeleteShader(shader);
            return 0;
        }

        return shader;
    }

    GLuint create_program(const char* source) {
        assert(source != nullptr);

        const auto shader = compile_shader(source);
        if (!shader) {
            return 0;
        }

        const auto program = glCreateProgram();
        glAttachShader(program, shader);

        glLinkProgram(program);
        glDeleteShader(shader);

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
    ComputeProgram::ComputeProgram() : _program(0), _local_size(1, 1, 1) {}

    ComputeProgram::~ComputeProgram() {
        if (_program != 0) {
            glDeleteProgram(_program);
        }
    }

    ComputeProgram::ComputeProgram(ComputeProgram&& other) noexcept :
        _program(other._program), _local_size(other._local_size) {
        other._program = 0;
        other._local_size = {1, 1, 1};
    }

    ComputeProgram& ComputeProgram::operator=(ComputeProgram&& other) noexcept {
        if (this != &other) {
            if (_program != 0) {
                glDeleteProgram(_program);
            }
            _program = other._program;
            _local_size = other._local_size;
            other._program = 0;
            other._local_size = {1, 1, 1};
        }
        return *this;
    }

    void ComputeProgram::load(const char* source) {
        const auto loaded_program = create_program(source);

        if (_program != 0) {
            glDeleteProgram(_program);
        }
        _program = loaded_program;

        auto local_size = std::array<GLint, 3>{1, 1, 1};
        glGetProgramiv(_program, GL_COMPUTE_WORK_GROUP_SIZE, local_size.data());
        _local_size = {local_size[0], local_size[1], local_size[2]};
    }

    void ComputeProgram::bind() const { glUseProgram(_program); }

    void ComputeProgram::unbind() { glUseProgram(0); }

    void ComputeProgram::dispatch(const GLuint width, const GLuint height) const {
        if (width == 0 || height == 0)
            return;

        const GLuint groups_x = (width + _local_size.x - 1) / _local_size.x;
        const GLuint groups_y = (height + _local_size.y - 1) / _local_size.y;

        glDispatchCompute(groups_x, groups_y, 1);
        glMemoryBarrier(GL_TEXTURE_FETCH_BARRIER_BIT | GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    }

    GLuint ComputeProgram::id() const { return _program; }

    void ComputeProgram::set_uniform_float(const char* name, const GLfloat value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform1f(location, value);
        }
    }

    void ComputeProgram::set_uniform_int(const char* name, const GLint value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform1i(location, value);
        }
    }

    void ComputeProgram::set_uniform_uint(const char* name, const GLuint value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform1ui(location, value);
        }
    }

    void ComputeProgram::set_uniform_vec2(const char* name, const GLfloat x, const GLfloat y) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform2f(location, x, y);
        }
    }

    void ComputeProgram::set_uniform_vec2(const char* name, const glm::vec2& value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform2fv(location, 1, glm::value_ptr(value));
        }
    }

    void ComputeProgram::set_uniform_vec3(const char* name, const GLfloat x, const GLfloat y, const GLfloat z) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform3f(location, x, y, z);
        }
    }

    void ComputeProgram::set_uniform_vec3(const char* name, const glm::vec3& value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform3fv(location, 1, glm::value_ptr(value));
        }
    }

    void ComputeProgram::set_uniform_vec4(const char* name, const GLfloat x, const GLfloat y, const GLfloat z,
                                          const GLfloat w) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform4f(location, x, y, z, w);
        }
    }

    void ComputeProgram::set_uniform_vec4(const char* name, const glm::vec4& value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform4fv(location, 1, glm::value_ptr(value));
        }
    }

    void ComputeProgram::set_uniform_ivec2(const char* name, const GLint x, const GLint y) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform2i(location, x, y);
        }
    }

    void ComputeProgram::set_uniform_ivec2(const char* name, const glm::ivec2& value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform2iv(location, 1, glm::value_ptr(value));
        }
    }

    void ComputeProgram::set_uniform_ivec3(const char* name, const GLint x, const GLint y, const GLint z) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform3i(location, x, y, z);
        }
    }

    void ComputeProgram::set_uniform_ivec3(const char* name, const glm::ivec3& value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform3iv(location, 1, glm::value_ptr(value));
        }
    }

    void ComputeProgram::set_uniform_ivec4(const char* name, const GLint x, const GLint y, const GLint z,
                                           const GLint w) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform4i(location, x, y, z, w);
        }
    }

    void ComputeProgram::set_uniform_ivec4(const char* name, const glm::ivec4& value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform4iv(location, 1, glm::value_ptr(value));
        }
    }

    void ComputeProgram::set_uniform_uvec2(const char* name, const GLuint x, const GLuint y) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform2ui(location, x, y);
        }
    }

    void ComputeProgram::set_uniform_uvec2(const char* name, const glm::uvec2& value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform2uiv(location, 1, glm::value_ptr(value));
        }
    }

    void ComputeProgram::set_uniform_uvec3(const char* name, const GLuint x, const GLuint y, const GLuint z) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform3ui(location, x, y, z);
        }
    }

    void ComputeProgram::set_uniform_uvec3(const char* name, const glm::uvec3& value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform3uiv(location, 1, glm::value_ptr(value));
        }
    }

    void ComputeProgram::set_uniform_uvec4(const char* name, const GLuint x, const GLuint y, const GLuint z,
                                           const GLuint w) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform4ui(location, x, y, z, w);
        }
    }

    void ComputeProgram::set_uniform_uvec4(const char* name, const glm::uvec4& value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniform4uiv(location, 1, glm::value_ptr(value));
        }
    }

    void ComputeProgram::set_uniform_mat2(const char* name, const glm::mat2& value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }
    }

    void ComputeProgram::set_uniform_mat3(const char* name, const glm::mat3& value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }
    }

    void ComputeProgram::set_uniform_mat4(const char* name, const glm::mat4& value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }
    }

    void ComputeProgram::set_uniform_mat2x3(const char* name, const glm::mat2x3& value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniformMatrix2x3fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }
    }

    void ComputeProgram::set_uniform_mat3x2(const char* name, const glm::mat3x2& value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniformMatrix3x2fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }
    }

    void ComputeProgram::set_uniform_mat2x4(const char* name, const glm::mat2x4& value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniformMatrix2x4fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }
    }

    void ComputeProgram::set_uniform_mat4x2(const char* name, const glm::mat4x2& value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniformMatrix4x2fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }
    }

    void ComputeProgram::set_uniform_mat3x4(const char* name, const glm::mat3x4& value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniformMatrix3x4fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }
    }

    void ComputeProgram::set_uniform_mat4x3(const char* name, const glm::mat4x3& value) const {
        if (const auto location = get_uniform_location(name); location != -1) {
            glUniformMatrix4x3fv(location, 1, GL_FALSE, glm::value_ptr(value));
        }
    }

    void ComputeProgram::set_uniform_float(const GLint location, const GLfloat value) { glUniform1f(location, value); }

    void ComputeProgram::set_uniform_int(const GLint location, const GLint value) { glUniform1i(location, value); }

    void ComputeProgram::set_uniform_uint(const GLint location, const GLuint value) { glUniform1ui(location, value); }

    void ComputeProgram::set_uniform_vec2(const GLint location, const GLfloat x, const GLfloat y) {
        glUniform2f(location, x, y);
    }

    void ComputeProgram::set_uniform_vec2(const GLint location, const glm::vec2& value) {
        glUniform2fv(location, 1, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_vec3(const GLint location, const GLfloat x, const GLfloat y, const GLfloat z) {
        glUniform3f(location, x, y, z);
    }

    void ComputeProgram::set_uniform_vec3(const GLint location, const glm::vec3& value) {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_vec4(const GLint location, const GLfloat x, const GLfloat y, const GLfloat z,
                                          const GLfloat w) {
        glUniform4f(location, x, y, z, w);
    }

    void ComputeProgram::set_uniform_vec4(const GLint location, const glm::vec4& value) {
        glUniform4fv(location, 1, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_ivec2(const GLint location, const GLint x, const GLint y) {
        glUniform2i(location, x, y);
    }

    void ComputeProgram::set_uniform_ivec2(const GLint location, const glm::ivec2& value) {
        glUniform2iv(location, 1, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_ivec3(const GLint location, const GLint x, const GLint y, const GLint z) {
        glUniform3i(location, x, y, z);
    }

    void ComputeProgram::set_uniform_ivec3(const GLint location, const glm::ivec3& value) {
        glUniform3iv(location, 1, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_ivec4(const GLint location, const GLint x, const GLint y, const GLint z,
                                           const GLint w) {
        glUniform4i(location, x, y, z, w);
    }

    void ComputeProgram::set_uniform_ivec4(const GLint location, const glm::ivec4& value) {
        glUniform4iv(location, 1, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_uvec2(const GLint location, const GLuint x, const GLuint y) {
        glUniform2ui(location, x, y);
    }

    void ComputeProgram::set_uniform_uvec2(const GLint location, const glm::uvec2& value) {
        glUniform2uiv(location, 1, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_uvec3(const GLint location, const GLuint x, const GLuint y, const GLuint z) {
        glUniform3ui(location, x, y, z);
    }

    void ComputeProgram::set_uniform_uvec3(const GLint location, const glm::uvec3& value) {
        glUniform3uiv(location, 1, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_uvec4(const GLint location, const GLuint x, const GLuint y, const GLuint z,
                                           const GLuint w) {
        glUniform4ui(location, x, y, z, w);
    }

    void ComputeProgram::set_uniform_uvec4(const GLint location, const glm::uvec4& value) {
        glUniform4uiv(location, 1, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_mat2(const GLint location, const glm::mat2& value) {
        glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_mat3(const GLint location, const glm::mat3& value) {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_mat4(const GLint location, const glm::mat4& value) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_mat2x3(const GLint location, const glm::mat2x3& value) {
        glUniformMatrix2x3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_mat3x2(const GLint location, const glm::mat3x2& value) {
        glUniformMatrix3x2fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_mat2x4(const GLint location, const glm::mat2x4& value) {
        glUniformMatrix2x4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_mat4x2(const GLint location, const glm::mat4x2& value) {
        glUniformMatrix4x2fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_mat3x4(const GLint location, const glm::mat3x4& value) {
        glUniformMatrix3x4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ComputeProgram::set_uniform_mat4x3(const GLint location, const glm::mat4x3& value) {
        glUniformMatrix4x3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    GLint ComputeProgram::get_uniform_location(const char* name) const {
        const auto location = glGetUniformLocation(_program, name);
        if (location == -1) {
            std::fprintf(stderr, "warning: uniform '%s' not found in program %u\n", name, _program);
        }
        return location;
    }
} // namespace imagegraph::process
