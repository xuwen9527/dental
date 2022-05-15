#include <engine.h>
#include <filesystem>
#include <reader_writer.h>
#include <ui/menubar.h>
#include "../external/imgui/imgui.h"
#include "../external/IGFD/ImGuiFileDialog.h"
#include <iostream>
#include <GLFW/glfw3.h>

namespace Dental::UI {
  MenuBar::MenuBar(Engine& engine, const std::string& name, bool visible) :
    View(engine, name, visible) {
  }

  void MenuBar::render() {
    if (!Visible) {
      return;
    }

    if (ImGui::BeginMainMenuBar()) {
      if (ImGui::BeginMenu(u8"File")) {        
        if (ImGui::MenuItem("Import", "CTRL+N")) {
          const char* filters = "support files (*.stl *.ply *.obj){.stl,.ply,.obj}";
          ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".stl", ImVec4(1.0f, 1.0f, 0.0f, 0.9f));
          ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".ply", ImVec4(1.0f, 1.0f, 0.0f, 0.9f));
          ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".obj", ImVec4(1.0f, 1.0f, 0.0f, 0.9f));
          IGFD::FileDialog::Instance()->OpenModal("ImportFileDialog", ICON_IGFD_FOLDER_OPEN "Import File", filters, "");
        }

        ImGui::Separator();

        if (ImGui::MenuItem(u8"Exit", "CTRL+X")) {
          glfwSetWindowShouldClose(engine_.window(), true);
        }
        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu(u8"View")) {
        auto& uiviews = engine_.uiviews();
        for (auto& uiview : uiviews) {
          if (ImGui::MenuItem(uiview->Name.c_str(), nullptr, &uiview->Visible)) {

          }
        }

        ImGui::EndMenu();
      }
    }
    ImGui::EndMainMenuBar();

    static std::string error_message;
    if (IGFD::FileDialog::Instance()->Display("ImportFileDialog", ImGuiWindowFlags_NoCollapse, ImVec2(400, 300))) {
      if (IGFD::FileDialog::Instance()->IsOk()) {
        std::string file_name = ImGuiFileDialog::Instance()->GetFilePathName();
        auto result = ReaderWriter::read(file_name);
        auto geometry = std::get<0>(result);
        if (geometry) {
          // error_message = std::get<2>(result);
        }
      }
      IGFD::FileDialog::Instance()->Close();
    }
  }
}