#include "../external/imgui/imgui.h"
#include <ui/undercut.h>
#include <engine.h>
#include <glm/gtx/euler_angles.hpp>

namespace Dental::UI {
  UnderCut::UnderCut(Engine& engine, const std::string& name, bool visible) :
    View(engine, name, visible) {
  }

  UnderCut::~UnderCut() {
  }

  void UnderCut::geometry(const GeometryPtr& geometry) {
    geometry_ = geometry;
    for (auto& viewer : viewers_) {
      viewer.scene()->clearGeometry();
      viewer.scene()->addGeometry(geometry);
    }
  }

  void UnderCut::render() {
    if (!Visible) {
      return;
    }

    if (!geometry_) {
      auto geom = engine_.viewer()->scene()->geometry(0);
      if (geom) {
        ShadowRenderTechniquePtr render = std::dynamic_pointer_cast<ShadowRenderTechnique>(geom->renderTechnique());
        if (render) {
          geometry(geom);
        }
      }
    }

    if (ImGui::Begin(Name.c_str(), &Visible)) {
      ImGui::Indent(ImGui::GetTreeNodeToLabelSpacing());
      if (ImGui::BeginTable("##undercut_table", 2, ImGuiTableFlags_SizingStretchProp)) {
        static glm::vec3 angle(0.f, 0.f, 0.f);
        if (geometry_) {
          ShadowRenderTechniquePtr render = std::dynamic_pointer_cast<ShadowRenderTechnique>(geometry_->renderTechnique());
          glm::mat4& mv = render->mv();
          glm::extractEulerAngleXYZ(mv, angle.x, angle.y, angle.z);
          angle = glm::degrees(angle);
        }

        ImGui::TableNextRow();
        {
          ImGui::TableSetColumnIndex(0);
          ImGui::Text("horizontal");
          ImGui::TableSetColumnIndex(1);
          ImGui::SetNextItemWidth(-FLT_MIN);
          if (ImGui::DragFloat("##undercut_horizontal", &angle.x, 0.01f)) {
            if (geometry_) {
              angle = glm::radians(angle);
              glm::quat rotate = glm::quat_cast(glm::eulerAngleXYZ(angle.x, angle.y, angle.z));
              ShadowRenderTechniquePtr render = std::dynamic_pointer_cast<ShadowRenderTechnique>(geometry_->renderTechnique());
              render->mv() = glm::mat4_cast(rotate);
            }
          }
        }
        ImGui::TableNextRow();
        {
          ImGui::TableSetColumnIndex(0);
          ImGui::Text("vertical");
          ImGui::TableSetColumnIndex(1);
          ImGui::SetNextItemWidth(-FLT_MIN);
          if (ImGui::DragFloat("##undercut_vertical", &angle.y, 0.01f)) {
            if (geometry_) {
              angle = glm::radians(angle);
              glm::quat rotate = glm::quat_cast(glm::eulerAngleXYZ(angle.x, angle.y, angle.z));
              ShadowRenderTechniquePtr render = std::dynamic_pointer_cast<ShadowRenderTechnique>(geometry_->renderTechnique());
              render->mv() = glm::mat4_cast(rotate);
            }
          }
        }

        ImGui::EndTable();
      }
      ImGui::Unindent(ImGui::GetTreeNodeToLabelSpacing());
      ImGui::NewLine();
    }
    ImGui::End();

    if (geometry_) {
      auto& viewport = engine_.viewer()->scene()->viewport();
      auto width = viewport.width() / 3;
      auto height = viewport.height() / 4;

      glm::quat quat(glm::identity<glm::quat>());
      quat = glm::rotate(quat, glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f));
      quat = glm::rotate(quat, glm::radians(-180.f), glm::vec3(0.f, 0.f, 1.f));

      for (int i = 0; i < viewers_.size(); ++i) {
        auto& viewer = viewers_[i];
        quat = glm::rotate(quat, glm::radians(90.f), glm::vec3(0.f, 0.f, 1.f));
        viewer.scene()->viewport(width * i, 0, width, height);
        viewer.home(0, quat, 0.5f);
        viewer.frame();
      }
    }
  }
}