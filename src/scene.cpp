#include <scene.h>

namespace Dental {
  Scene::Scene() : Camera() {
    perspectiveViewport(0, 0, 400, 300);
  }

  Scene::~Scene() {

  }

  Scene& Scene::operator = (const Scene& rhs) {
    if (this != &rhs) {
      (Camera&)(*this) = (Camera&)rhs;
    }
    return *this;
  }

  Scene& Scene::operator = (Scene&& rhs) noexcept {
    if (this != &rhs) {
      (Camera&)(*this) = std::move((Camera&)rhs);
    }
    return *this;
  }

  Scene::Scene(const Scene& rhs) {
    *this = rhs;
  }

  Scene::Scene(Scene&& rhs) noexcept {
    *this = std::move(rhs);
  }

  void Scene::accept(Visitor& visitor) {
    visitor.apply(*this);
  }
}