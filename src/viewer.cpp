#include <glad/glad.h>
#include <render_visitor.h>
#include <viewer.h>

namespace Dental {
  Viewer::Viewer() :
    scene_(std::make_shared<Scene>()),
    manipulator_(std::make_shared<Manipulator>()),
    move_event_(Event::POINTER_MOVE, Event::LEFT_BUTTON, 0.f, 0.f) {
    manipulator_->camera(std::dynamic_pointer_cast<Camera>(scene_));
  }

  Viewer::~Viewer() {

  }

  void Viewer::home(float duration_s, const glm::quat& quat, float zoom) {
    scene_->dirtyBounding();

    Manipulator::Viewpoint viewpoint = 
      manipulator_->createViewpoint(scene_->boundingSphere());
    if (viewpoint.valid()) {
        viewpoint.quat(quat);
        viewpoint.range(viewpoint.range() * zoom);
        manipulator_->viewpoint(viewpoint, duration_s);
    }
  }

  void Viewer::frame() {
    if (!manipulator_->valid()) {
      home();
    }

    RenderInfoPtr render_info = std::make_shared<RenderInfo>();
    manipulator_->apply(render_info);

    if (!events_.empty()) {
      Event event;
      events_.pop(event);

      if (event.type() & Event::POINTER) {
        handleEvent(event);
      }
    }

    render(render_info);
  }

  void Viewer::render(RenderInfoPtr& render_info) {
    RenderVisitor visitor(render_info);
    scene_->accept(visitor);
  }

  bool Viewer::handleEvent(Event &event) {
    return manipulator_->handleEvent(event);
  }
}