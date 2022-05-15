#include "../external/imgui/imgui.h"
#include <ui/preview.h>
#include <engine.h>

namespace Dental::UI {
  Preview::Preview(Engine& engine, const std::string& name, bool visible) :
    View(engine, name, visible) {
  }

  Preview::~Preview() {
  }

  void Preview::render_to_frame_buffer(unsigned int width, unsigned int height) {

  }

  void Preview::render() {
    if (!Visible) {
      return;
    }

    // ImGui::SetNextWindowBgAlpha(0);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.f);

    if (ImGui::Begin(Name.c_str(), &Visible)) {
      auto preview_origin = ImGui::GetCursorScreenPos();
      auto preview_size   = ImGui::GetContentRegionAvail();
      auto preview_corner = ImVec2(preview_origin.x + preview_size.x, preview_origin.y + preview_size.y);

      if (ImGui::BeginChild("canvas", ImVec2(0, 0), false)) {
      }
      ImGui::EndChild();
    }
    ImGui::End();

    ImGui::PopStyleVar(3);
  }
}