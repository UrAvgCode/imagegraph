#include <imagegraph/compute/shader_storage_buffer.h>

#include <cassert>

namespace imagegraph::compute {
    ShaderStorageBuffer::ShaderStorageBuffer() : _id(0), _size(0) {}

    ShaderStorageBuffer::~ShaderStorageBuffer() {
        if (_id) {
            glDeleteBuffers(1, &_id);
        }
    }

    ShaderStorageBuffer::ShaderStorageBuffer(ShaderStorageBuffer&& other) noexcept :
        _id(other._id), _size(other._size) {
        other._id = 0;
        other._size = 0;
    }

    ShaderStorageBuffer& ShaderStorageBuffer::operator=(ShaderStorageBuffer&& other) noexcept {
        if (this != &other) {
            if (_id) {
                glDeleteBuffers(1, &_id);
            }

            _id = other._id;
            _size = other._size;
            other._id = 0;
            other._size = 0;
        }

        return *this;
    }

    void ShaderStorageBuffer::allocate(const GLsizeiptr size) {
        if (_size == size) {
            return;
        }

        if (_id) {
            glDeleteBuffers(1, &_id);
        }

        _size = size;

        glCreateBuffers(1, &_id);
        glNamedBufferStorage(_id, _size, nullptr, GL_DYNAMIC_STORAGE_BIT);
    }

    void ShaderStorageBuffer::clear() const {
        assert(_id != 0);
        constexpr GLubyte zero = 0;
        glClearNamedBufferData(_id, GL_R8UI, GL_RED_INTEGER, GL_UNSIGNED_BYTE, &zero);
    }

    void ShaderStorageBuffer::bind_base(const GLuint binding) const {
        assert(_id != 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, _id);
    }

    GLuint ShaderStorageBuffer::id() const { return _id; }

    GLsizeiptr ShaderStorageBuffer::size() const { return _size; }
} // namespace imagegraph::compute
