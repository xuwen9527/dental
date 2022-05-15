#ifndef __SCENE_H__
#define __SCENE_H__

#include <camera.h>

namespace Dental {
  class Scene : public Camera {
  public:
    Scene();
    virtual ~Scene() override;

    Mate_Node(Dental, Scene)

    Scene& operator = (const Scene&);
    Scene& operator = (Scene&& rhs) noexcept;
    Scene(const Scene&);
    Scene(Scene&&) noexcept;

    virtual void accept(Visitor& visitor) override;
  };

  using ScenePtr = std::shared_ptr<Scene>;
}
#endif