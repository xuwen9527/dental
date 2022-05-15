#include <primitive_set.h>

#define ENABLE_BUFFER

namespace Dental {
  unsigned int PrimitiveSet::modeSize() const {
    switch (mode_) {
    case (Mode::LINES):
      return 2;
    case (Mode::TRIANGLES):
      return 3;
    case (Mode::POINTS):
    case (Mode::LINE_STRIP):
    case (Mode::LINE_LOOP):
    case (Mode::TRIANGLE_STRIP):
    case (Mode::TRIANGLE_FAN):
      return 1;
    default:
      return 1;
    }
  }

  unsigned int PrimitiveSet::numPrimitives() const {
    return numIndices() / modeSize();
  }

  GLElementArrayObject::GLElementArrayObject() {
    mode_ = GL_POINTS;
    first_ = 0;
    count_ = 0;
  }

  void GLElementArrayObject::bind(GLenum mode, GLint first, GLsizei count) {
    mode_ = mode;
    first_ = first;
    count_ = count;
  }

  void GLElementArrayObject::bind() {
    if (count_) {
      glDrawArrays(mode_, first_, count_);
    }
  }

  void DrawArrays::bind() {
    if (count_) {
      gl_object_->bind((GLenum)mode_, first_, count_);
    }
  }

  PrimitiveSetPtr DrawArrays::clone() {
    DrawArraysPtr arrays = std::make_shared<DrawArrays>();
    arrays->mode_ = mode_;
    arrays->primitive_type_ = primitive_type_;
    arrays->first_ = first_;
    arrays->count_ = count_;
    dirty();
    return arrays;
  }

  GLElementBufferObject::GLElementBufferObject(short data_type_size, unsigned long gl_data_type) {
    profile_.data_type_size = data_type_size;
    profile_.gl_data_type = gl_data_type;

    mode_ = 0;
    ebo_ = 0;
    data_size_ = 0;
    data_ = 0;
    dirty_ = true;
  }

  void GLElementBufferObject::bind(GLenum mode, unsigned long data_size, void* data) {
    mode_ = mode;
    data_size_ = data_size;
    data_ = data;
  }

  void GLElementBufferObject::bind() {
    if (!data_size_) {
      dirty_ = false;
      return;
    }
#ifdef ENABLE_BUFFER
    if (!ebo_) {
      glGenBuffers(1, &ebo_);
      dirty_ = true;
    }

    if (!ebo_) {
      return;
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);

    if (dirty_) {
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)data_size_ * profile_.data_type_size, data_,
                   GL_STATIC_DRAW);
      dirty_ = false;
    }

    if (data_size_) {
      glDrawElements(mode_, data_size_, profile_.gl_data_type, 0);
    }
#else
    if (data_size_) {
      glDrawElements(mode_, data_size_, profile_.gl_data_type, data_);
    }
#endif
  }

  void GLElementBufferObject::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }

  void GLElementBufferObject::release() {
    if (ebo_) {
      glDeleteBuffers(1, &ebo_);
      ebo_ = 0;
    }
  }

  void GLElementBufferObject::dirty() {
    dirty_ = true;
  }

  bool GLElementBufferObject::valid() {
    return !dirty_ && data_size_ && ebo_ != 0;
  }
}