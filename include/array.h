#ifndef __ARRAY_H__
#define __ARRAY_H__

#include <vector>
#include <memory>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glad/glad.h>
#include <gl_object.h>

namespace Dental {
  class GLArrayObject : public GLObject {
  public:
    GLArrayObject(short data_type_size, short gl_data_size, unsigned long gl_data_type);
    ~GLArrayObject() override { release(); }

    GLArrayObject& operator = (GLArrayObject&&) noexcept = delete;
    GLArrayObject& operator = (const GLArrayObject&) = delete;
    GLArrayObject(const GLArrayObject&) = delete;
    GLArrayObject(GLArrayObject&&) noexcept = delete;
  
    void bindIndex(unsigned int index);

    void bindData(GLsizeiptr data_size, void *data);

    virtual void bind() override;

    virtual void unbind() override;

    virtual void release() override;

    virtual void dirty() override;

    virtual bool valid() override;

  private:
    struct Profile {
      short data_type_size;
      short gl_data_size;
      unsigned long gl_data_type;
    } profile_;

    int index_;
    unsigned int vbo_;
    GLsizeiptr data_size_;
    void* data_;
    bool dirty_;
  };

  using GLArrayObjectPtr = std::shared_ptr<GLArrayObject>;

  template<typename TYPE, unsigned long GLTYPE, unsigned int GLSIZE>
  class Array : public std::vector<TYPE> {
  public:
    using base_type = std::vector<TYPE>;
    using self_type = Array<TYPE, GLTYPE, GLSIZE>;

    Array() : gl_object_(std::make_shared<GLArrayObject>(sizeof(TYPE), GLSIZE, GLTYPE)) {
    }

    Array(const self_type &rhs) {
      if (this != &rhs) {
        *this = rhs;
      }
    }

    Array(self_type&& rhs) noexcept {
      if (this != &rhs) {
        *this = std::move(rhs);
      }
    }

    ~Array() { clear(); }

    self_type& operator = (const self_type& rhs) {
      base_type::reserve(rhs.size());
      base_type::assign(rhs.begin(), rhs.end());
      dirty();
      return *this;
    }
  
    self_type& operator = (self_type&& rhs) noexcept {
      (base_type)(*this) = std::move(rhs);
      dirty();
      rhs.dirty();
      return *this;
    }
  
    inline void clear() {
      base_type().swap(*this);
      dirty();
    }

    inline void bind(unsigned int index) {
      gl_object_->bindIndex(index);
      gl_object_->bindData(base_type::size(), base_type::data());
    }

    inline void dirty() {
      gl_object_->bindData(base_type::size(), base_type::data());
      gl_object_->dirty();
    }

    GLObjectPtr GLObject() const {
      return gl_object_;
    }

  private:
    GLArrayObjectPtr gl_object_;
  };

  using Vec2Array = Array<glm::vec2, GL_FLOAT, 2>;
  using Vec2ArrayPtr = std::shared_ptr<Vec2Array>;

  using Vec3Array = Array<glm::vec3, GL_FLOAT, 3>;
  using Vec3ArrayPtr = std::shared_ptr<Vec3Array>;

  using Vec4Array = Array<glm::vec4, GL_FLOAT, 4>;
  using Vec4ArrayPtr = std::shared_ptr<Vec4Array>;
}
#endif