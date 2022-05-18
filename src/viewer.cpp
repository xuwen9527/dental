#include <glad/glad.h>
#include <render_visitor.h>
#include <viewer.h>

namespace Dental {
  Viewer::Viewer() :
    scene_(std::make_shared<Scene>()),
    manipulator_(std::make_shared<Manipulator>()) {
    manipulator_->camera(std::dynamic_pointer_cast<Camera>(scene_));
  }

  Viewer::~Viewer() {

  }

  void Viewer::home(float duration_s) {
    scene_->dirtyBounding();

    Manipulator::Viewpoint viewpoint = 
      manipulator_->createViewpoint(scene_->boundingSphere());
    if (viewpoint.valid()) {
        manipulator_->viewpoint(viewpoint, duration_s);
        events_.redraw();
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

      if (event.type() == Event::RENDER) {
        // render(render_info);
      } else if (event.type() & Event::POINTER) {
        if (handleEvent(event)) {
          // render(render_info);
        }
      }
    }

    render(render_info);
  }

  void Viewer::render(RenderInfoPtr& render_info) {
    scene_->viewport().apply();

    glClearColor(0.45f, 0.55f, 0.60f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    RenderVisitor visitor(render_info);
    scene_->accept(visitor);
  }

  bool Viewer::handleEvent(Event &event) {
    scene_->viewport().windowToProject(event.firstProjectPoint(), event.firstPoint());
    scene_->viewport().windowToProject(event.secondProjectPoint(), event.secondPoint());

    return manipulator_->handleEvent(event);
  }
}