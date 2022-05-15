#ifndef __PRIMITIVE_SET_H__
#define __PRIMITIVE_SET_H__

#include <vector>
#include <memory>
#include <glad/glad.h>
#include <gl_object.h>

namespace Dental {
  class PrimitiveSet;
  using PrimitiveSetPtr = std::shared_ptr<PrimitiveSet>;

  class PrimitiveSet {
  public:
    enum class Type {
      PRIMITIVE_TYPE,
      DRAW_ARRAYS,
      DRAW_ELEMENTS_UINT
    };

    enum class Mode {
      POINTS = GL_POINTS,
      LINES = GL_LINES,
      LINE_STRIP = GL_LINE_STRIP,
      LINE_LOOP = GL_LINE_LOOP,
      TRIANGLES = GL_TRIANGLES,
      TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
      TRIANGLE_FAN = GL_TRIANGLE_FAN,
    };

    PrimitiveSet(Type primitiveType = Type::PRIMITIVE_TYPE, Mode mode = Mode::LINE_LOOP) :
      primitive_type_(primitiveType), mode_(mode) {}

    virtual ~PrimitiveSet() {}

    PrimitiveSet& operator = (PrimitiveSet&&) noexcept = delete;
    PrimitiveSet& operator = (const PrimitiveSet&) = delete;
    PrimitiveSet(const PrimitiveSet&) = delete;
    PrimitiveSet(PrimitiveSet&&) noexcept = delete;

    inline Type type() const { return primitive_type_; }

    inline void mode(Mode mode) { mode_ = mode; }
    inline Mode mode() const { return mode_; }

    unsigned int modeSize() const;

    virtual void bind() = 0;

    virtual unsigned int index(unsigned int pos) const = 0;

    virtual unsigned int numIndices() const = 0;

    virtual unsigned int numPrimitives() const;

    virtual void reversePrimitives(unsigned int size) = 0;

    virtual void emplace(unsigned int index) = 0;

    void emplace(const std::initializer_list<unsigned int>& list) {
      for (const auto& index : list) emplace(index);
    }

    virtual void dirty() = 0;

    virtual PrimitiveSetPtr clone() = 0;

    virtual GLObjectPtr GLObject() const = 0;

  protected:
    Type primitive_type_;
    Mode mode_;
  };

  using PrimitiveSetPtr = std::shared_ptr<PrimitiveSet>;

  class GLElementArrayObject : public GLObject {
  public:
    GLElementArrayObject();
    ~GLElementArrayObject() override { release(); }

    GLElementArrayObject& operator = (GLElementArrayObject&&) noexcept = delete;
    GLElementArrayObject& operator = (const GLElementArrayObject&) = delete;
    GLElementArrayObject(const GLElementArrayObject&) = delete;
    GLElementArrayObject(GLElementArrayObject&&) noexcept = delete;

    virtual void bind(GLenum mode, GLint first, GLsizei count);

    virtual void bind() override;

    virtual void unbind() override {}

    virtual void release() override {}

    virtual void dirty() override {}

    virtual bool valid() override { return count_ > 0; }

  private:
    GLenum mode_;
    GLint first_;
    GLsizei count_;
  };

  using GLElementArrayObjectPtr = std::shared_ptr<GLElementArrayObject>;

  class DrawArrays : public PrimitiveSet {
  public:
    DrawArrays(Mode mode = Mode::LINE_LOOP) :
      PrimitiveSet(Type::DRAW_ARRAYS, mode),
      first_(0), count_(0),
      gl_object_(std::make_shared<GLElementArrayObject>()) {}

    DrawArrays(Mode mode, GLint first, GLsizei count) :
      PrimitiveSet(Type::DRAW_ARRAYS, mode),
      first_(first), count_(count),
      gl_object_(std::make_shared<GLElementArrayObject>()) {}

    virtual ~DrawArrays() override {}

    DrawArrays& operator = (DrawArrays&&) noexcept = delete;
    DrawArrays& operator = (const DrawArrays&) = delete;
    DrawArrays(const DrawArrays&) = delete;
    DrawArrays(DrawArrays&&) noexcept = delete;

    inline void set(Mode mode, GLint first, GLsizei count) {
      mode_ = mode;
      first_ = first;
      count_ = count;
    }

    inline void first(GLint first) { first_ = first; }
    inline GLint first() const { return first_; }

    inline void count(GLsizei count) { count_ = count; }
    inline GLsizei count() const { return count_; }

    virtual void bind() override;

    inline virtual unsigned int numIndices() const override {
      return static_cast<unsigned int>(count_);
    }

    inline virtual void reversePrimitives(unsigned int size) override {
      count_ = size;
    }

    inline virtual unsigned int index(unsigned int pos) const override {
      return static_cast<unsigned int>(first_) + pos;
    }

    virtual void emplace(unsigned int index) override {
      if ((unsigned int)count_ <= (index + 1)) {
        count_ = index + 1;
      }
    }

    inline virtual void dirty() override {
      bind();
    }

    virtual PrimitiveSetPtr clone() override;

    GLObjectPtr GLObject() const override {
      return gl_object_;
    }

  protected:
    GLint first_;
    GLsizei count_;

    GLElementArrayObjectPtr gl_object_;
  };

  using DrawArraysPtr = std::shared_ptr<DrawArrays>;

  class GLElementBufferObject : public GLObject {
  public:
    GLElementBufferObject(short data_type_size, unsigned long gl_data_type);
    ~GLElementBufferObject() override { release(); }

    GLElementBufferObject& operator = (GLElementBufferObject&&) noexcept = delete;
    GLElementBufferObject& operator = (const GLElementBufferObject&) = delete;
    GLElementBufferObject(const GLElementBufferObject&) = delete;
    GLElementBufferObject(GLElementBufferObject&&) noexcept = delete;

    virtual void bind(GLenum mode, unsigned long data_size, void *data);

    virtual void bind() override;

    virtual void unbind() override;

    virtual void release() override;

    virtual void dirty() override;

    virtual bool valid() override;

  private:
    struct Profile {
      short data_type_size;
      unsigned long gl_data_type;
    } profile_;

    GLenum mode_;
    unsigned int ebo_;
    GLsizei data_size_;
    void* data_;
    bool dirty_;
  };

  using GLElementBufferObjectPtr = std::shared_ptr<GLElementBufferObject>;

  template<typename TYPE, unsigned long GLTYPE>
  class DrawElements : public PrimitiveSet, public std::vector<TYPE> {
  public:
    using vector_type = std::vector<TYPE>;

    DrawElements(Mode mode = Mode::LINE_STRIP) :
      PrimitiveSet(Type::DRAW_ELEMENTS_UINT, mode),
      gl_object_(std::make_shared<GLElementBufferObject>(sizeof(TYPE), GLTYPE)) {}

    template<typename data_type>
    DrawElements(Mode mode, unsigned int no, const data_type *ptr) :
      PrimitiveSet(Type::DRAW_ELEMENTS_UINT, mode),
      vector_type(ptr, ptr + no),
      gl_object_(std::make_shared<GLElementBufferObject>(sizeof(TYPE), GLTYPE)) {}

    DrawElements(Mode mode, unsigned int no) :
      PrimitiveSet(Type::DRAW_ELEMENTS_UINT, mode),
      vector_type(no),
      gl_object_(std::make_shared<GLElementBufferObject>(sizeof(TYPE), GLTYPE)) {}

    template<class InputIterator>
    DrawElements(Mode mode, InputIterator first, InputIterator last) :
      PrimitiveSet(Type::DRAW_ELEMENTS_UINT, mode),
      vector_type(first, last),
      gl_object_(std::make_shared<GLElementBufferObject>(sizeof(TYPE), GLTYPE)) {}

    virtual ~DrawElements() override { clear(); }

    DrawElements& operator = (DrawElements&&) noexcept = delete;
    DrawElements& operator = (const DrawElements&) = delete;
    DrawElements(const DrawElements&) = delete;
    DrawElements(DrawElements&&) noexcept = delete;

    inline void clear() { std::vector<TYPE>::clear(); }

    inline virtual unsigned int numIndices() const override {
      return static_cast<unsigned int>(vector_type::size());
    }

    inline virtual void reversePrimitives(unsigned int size) override {
      vector_type::reserve(modeSize() * size);
    }

    inline virtual unsigned int index(unsigned int pos) const override {
      return (*this)[pos];
    }

    inline virtual void emplace(unsigned int index) override {
      vector_type::emplace_back((TYPE)(index));
    }

    inline virtual void bind() override {
      gl_object_->bind(
        static_cast<std::underlying_type<Mode>::type>(mode_),
        vector_type::size(),
        vector_type::data()
      );
    }

    inline virtual void dirty() override {
      bind();
      gl_object_->dirty();
    }

    virtual PrimitiveSetPtr clone() override {
      std::shared_ptr<DrawElements> elements = std::make_shared<DrawElements<TYPE, GLTYPE>>();
      elements->mode_ = mode_;
      elements->primitive_type_ = primitive_type_;
      elements->assign(vector_type::begin(), vector_type::end());
      dirty();
      return elements;
    }

    inline GLObjectPtr GLObject() const override {
      return gl_object_;
    }

  protected:
    GLElementBufferObjectPtr gl_object_;
  };

  using DrawElementsUInt = DrawElements<GLuint, GL_UNSIGNED_INT>;
  using DrawElementsUIntPtr = std::shared_ptr<DrawElementsUInt>;

  using DrawElementsUShort = DrawElements<GLushort, GL_UNSIGNED_SHORT>;
  using DrawElementsUShortPtr = std::shared_ptr<DrawElementsUShort>;
}
#endif