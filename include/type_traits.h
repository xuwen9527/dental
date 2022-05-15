#ifndef __TYPE_TRAITS_H__
#define __TYPE_TRAITS_H__

#include <string>

namespace Dental {
  template <typename T>
  struct TypeNameTraits {
    constexpr static const char* value = "";
  };

#define DECLARE_TYPETRAITS(type_name, type)                   \
  template <>                                                 \
  struct TypeNameTraits<type> {               \
    constexpr static const char* value = #type_name;          \
  };

  DECLARE_TYPETRAITS(Bool, bool)
  DECLARE_TYPETRAITS(Int, int)
  DECLARE_TYPETRAITS(Float, float)
  DECLARE_TYPETRAITS(String, std::string)
  DECLARE_TYPETRAITS(IntVec2, glm::ivec2)
  DECLARE_TYPETRAITS(IntVec3, glm::ivec3)
  DECLARE_TYPETRAITS(IntVec4, glm::ivec4)
  DECLARE_TYPETRAITS(Vec2, glm::vec2)
  DECLARE_TYPETRAITS(Vec3, glm::vec3)
  DECLARE_TYPETRAITS(Vec4, glm::vec4)
  DECLARE_TYPETRAITS(Mat2, glm::mat2)
  DECLARE_TYPETRAITS(Mat3, glm::mat3)
  DECLARE_TYPETRAITS(Mat4, glm::mat4)
}
#endif