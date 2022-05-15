#include <glm/ext.hpp>
#include <glad/glad.h>
#include <uniform.h>

namespace Dental {
  IMPLEMENT_UNIFORM(Bool, bool) {
    glUniform1i(location, value_);
  }

  IMPLEMENT_UNIFORM(Int, int) {
    glUniform1i(location, value_);
  }

  IMPLEMENT_UNIFORM(Float, float) {
    glUniform1f(location, value_);
  }

  IMPLEMENT_UNIFORM(IntVec2, glm::ivec2) {
    glUniform2iv(location, 1, glm::value_ptr(value_));
  }

  IMPLEMENT_UNIFORM(IntVec3, glm::ivec3) {
    glUniform3iv(location, 1, glm::value_ptr(value_));
  }

  IMPLEMENT_UNIFORM(IntVec4, glm::ivec4) {
    glUniform4iv(location, 1, glm::value_ptr(value_));
  }

  IMPLEMENT_UNIFORM(Vec2, glm::vec2) {
    glUniform2fv(location, 1, glm::value_ptr(value_));
  }

  IMPLEMENT_UNIFORM(Vec3, glm::vec3) {
    glUniform3fv(location, 1, glm::value_ptr(value_));
  }

  IMPLEMENT_UNIFORM(Vec4, glm::vec4) {
    glUniform4fv(location, 1, glm::value_ptr(value_));
  }

  IMPLEMENT_UNIFORM(Mat2, glm::mat2) {
    glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value_));
  }

  IMPLEMENT_UNIFORM(Mat3, glm::mat3) {
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value_));
  }

  IMPLEMENT_UNIFORM(Mat4, glm::mat4) {
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value_));
  }
}