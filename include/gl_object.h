#ifndef __GL_OBJECT_H__
#define __GL_OBJECT_H__

#include <memory>
#include <list>

namespace Dental {
  class GLObject {
  public:
    GLObject() {}
    virtual ~GLObject() {};

    GLObject& operator = (GLObject&&) noexcept = delete;
    GLObject& operator = (const GLObject&) = delete;
    GLObject(const GLObject&) = delete;
    GLObject(GLObject&&) noexcept = delete;

    virtual void bind() = 0;

    virtual void unbind() = 0;

    virtual void release() = 0;

    virtual void dirty() = 0;

    virtual bool valid() = 0;
  };

  using GLObjectPtr = std::shared_ptr<GLObject>;

  class GLObjectPtrs : public std::list<GLObjectPtr> {
  public:
    GLObjectPtrs() : std::list<GLObjectPtr>() {}

    GLObjectPtrs& operator = (GLObjectPtrs&&) noexcept = delete;
    GLObjectPtrs& operator = (const GLObjectPtrs&) = delete;
    GLObjectPtrs(const GLObjectPtrs&) = delete;
    GLObjectPtrs(GLObjectPtrs&&) noexcept = delete;

    void bind() {
      for (iterator itr = begin(); itr != end(); ++itr) {
        (*itr)->bind();
      }
    }

    void unbind() {
      for (iterator itr = begin(); itr != end(); ++itr) {
        (*itr)->unbind();
      }
    }

    inline void render() {
      bind();
      unbind();
    }

    void release() {
      for (reverse_iterator itr = rbegin(); itr != rend(); ++itr) {
        (*itr)->release();
      }
    }

    void clean() {
      for (iterator itr = begin(); itr != end();) {
        if ((*itr)->valid())
          ++itr;
        else
          itr = erase(itr);
      }
    }
  };
}
#endif