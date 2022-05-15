#include <array.h>

#define ENABLE_BUFFER true

namespace Dental {
  GLArrayObject::GLArrayObject(
    short data_type_size, short gl_data_size,
    unsigned long gl_data_type) :
    data_size_(0),
    data_(nullptr),
    vbo_(0),
    index_(-1),
    dirty_(false) {
    profile_.gl_data_size = gl_data_size;
    profile_.gl_data_type = gl_data_type;
    profile_.data_type_size = data_type_size;
  }

  void GLArrayObject::bindIndex(unsigned int index) {
    index_ = index;
  }

  void GLArrayObject::bindData(GLsizeiptr data_size, void* data) {
    data_size_ = data_size;
    data_ = data;
  }

  void GLArrayObject::bind() {
    if (!data_size_) {
      dirty_ = false;
      return;
    }
#ifdef ENABLE_BUFFER
    if (!vbo_) {
      glGenBuffers(1, &vbo_);
      dirty_ = true;
    }

    if (!vbo_) {
      return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

    if (dirty_) {
      glBufferData(GL_ARRAY_BUFFER, data_size_ * profile_.data_type_size, data_,
                   GL_STATIC_DRAW);
      dirty_ = false;
    }

    if (index_ != -1) {
      glVertexAttribPointer(index_, profile_.gl_data_size, profile_.gl_data_type, GL_FALSE,
                            profile_.data_type_size, 0);
      glEnableVertexAttribArray(index_);
    }
#else
    if (index_ != -1) {
      glVertexAttribPointer(index_, profile_.gl_data_size, profile_.gl_data_type, GL_FALSE,
                            profile_.data_type_size, data_);
      glEnableVertexAttribArray(index_);
    }
#endif
  }

  void GLArrayObject::unbind() {
    if (index_ != -1) {
      glDisableVertexAttribArray(index_);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }

  void GLArrayObject::release() {
    if (vbo_) {
      glDeleteBuffers(1, &vbo_);
      vbo_ = 0;
    }
  }

  void GLArrayObject::dirty() {
    dirty_ = true;
  }

  bool GLArrayObject::valid() {
    return !dirty_ && index_ != -1 && vbo_ != 0;
  }
}