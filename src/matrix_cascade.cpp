#include <glm/ext.hpp>
#include <matrix_cascade.h>

namespace Dental {
  MatrixCascade::MatrixCascade() {
  }

  MatrixCascade::~MatrixCascade() {

  }

  MatrixCascade& MatrixCascade::operator = (const MatrixCascade& rhs) {
    if (this != &rhs) {
      (parent_type&)(*this) = (parent_type&)rhs;
    }
    return *this;
  }

  MatrixCascade& MatrixCascade::operator = (MatrixCascade&& rhs) noexcept {
    if (this != &rhs) {
      (parent_type&)*this = std::move((parent_type&)rhs);
    }
    return *this;
  }

  MatrixCascade::MatrixCascade(const MatrixCascade& rhs) {
    *this = rhs;
  }

  MatrixCascade::MatrixCascade(MatrixCascade&& rhs) noexcept {
    *this = std::move(rhs);
  }

  void MatrixCascade::push(const glm::mat4& matrix) {
    if (empty()) {
      emplace(matrix);
    } else {
      emplace(top() * matrix);
    }
  }
}