#ifndef __UNIFORM_H__
#define __UNIFORM_H__

#include <string>
#include <memory>
#include <glm/ext.hpp>
#include <type_traits.h>

namespace Dental {
  class Uniform {
  public:
    Uniform() {}
    Uniform(const std::string& name) : name_(name) {
    }

    Uniform& operator = (Uniform&&) noexcept = delete;
    Uniform& operator = (const Uniform&) = delete;
    Uniform(const Uniform&) = delete;
    Uniform(Uniform&&) noexcept = delete;

    virtual std::string typeName() const = 0;

    virtual void bind(int location) const = 0;

    inline const std::string& name() const { return name_; }

    template <typename T>
    inline const T& value() const {
      // static_assert(false, "Uniform do not support this type.");
      return T();
    }

    template <typename T>
    inline void value(const T& value) {
      // static_assert(false, "Uniform do not support this type.");
    }

  protected:
    std::string name_;
  };

  using UniformPtr = std::shared_ptr<Uniform>;

  template <typename T>
  class UniformT : public Uniform {
  public:
    using value_type = T;
    UniformT() : value_(T()) {}
    explicit UniformT(const std::string& name, const T& value) :
      Uniform(name), value_(value) {
    }

    UniformT& operator = (const UniformT& rhs) {
      if (this != &rhs) {
        name_ = rhs.name_;
        value_ = rhs.value_;
      }
      return *this;
    }

    UniformT& operator = (UniformT&& rhs) noexcept {
      if (this != &rhs) {
        name_ = std::move(rhs.name_);
        value_ = std::move(rhs.value_);
      }
      return *this;
    }

    UniformT(const UniformT& rhs) {
      *this = rhs;
    }

    UniformT(UniformT&& rhs) noexcept {
      *this = std::move(rhs);
    }

    std::string typeName() const override {
      return TypeNameTraits<T>::value;
    }

    const T& value() const { return value_; }
  
    void value(const T& value) { value_ = value; }

    void reset() { value_ = T(); }    

    void bind(int location) const override;

  protected:
    T value_;
  };

#define DECLARE_UNIFORM(name, type)                              \
  template <> const type& Uniform::value<type>() const;          \
  template <> void Uniform::value<type>(const type& value);      \
  using Uniform##name = UniformT<type>;

#define IMPLEMENT_UNIFORM(name, type)                            \
  template<> class UniformT<type>;                               \
  template <>                                                    \
  const type& Uniform::value<type>() const {                     \
    return static_cast<const UniformT<type>*>(this)->value();    \
  }                                                              \
  template <>                                                    \
  void Uniform::value<type>(const type& value) {                 \
    static_cast<UniformT<type>*>(this)->value(value);            \
  }                                                              \
  template<>                                                     \
  void UniformT<type>::bind(int location) const

DECLARE_UNIFORM(Bool,    bool)
DECLARE_UNIFORM(Int,     int)
DECLARE_UNIFORM(Float,   float)
DECLARE_UNIFORM(IntVec2, glm::ivec2)
DECLARE_UNIFORM(IntVec3, glm::ivec3)
DECLARE_UNIFORM(IntVec4, glm::ivec4)
DECLARE_UNIFORM(Vec2,    glm::vec2)
DECLARE_UNIFORM(Vec3,    glm::vec3)
DECLARE_UNIFORM(Vec4,    glm::vec4)
DECLARE_UNIFORM(Mat2,    glm::mat2)
DECLARE_UNIFORM(Mat3,    glm::mat3)
DECLARE_UNIFORM(Mat4,    glm::mat4)
}
#endif