#ifndef __MATRIXCASCADE_H__
#define __MATRIXCASCADE_H__

#include <stack>
#include <glm/mat4x4.hpp>

namespace Dental {
  class MatrixCascade : public std::stack<glm::mat4> {
  public:
    using parent_type = std::stack<glm::mat4>;
    MatrixCascade();
    ~MatrixCascade();

    MatrixCascade& operator = (const MatrixCascade& rhs);
    MatrixCascade& operator = (MatrixCascade&& rhs) noexcept;
    MatrixCascade(const MatrixCascade& rhs);
    MatrixCascade(MatrixCascade&& rhs) noexcept;

    void push(const glm::mat4& matrix);
  };
}
#endif