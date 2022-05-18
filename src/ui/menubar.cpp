#include <iostream>
#include <filesystem>
#include <engine.h>
#include <reader_writer.h>
#include <ui/menubar.h>
#include <GLFW/glfw3.h>
#include "../external/imgui/imgui.h"
#include "../external/ifd/ImFileDialog.h"

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
          ifd::FileDialog::Instance().Open("ImportFileDialog", "Import File", filters);
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
    if (ifd::FileDialog::Instance().IsDone("ImportFileDialog")) {
      if (ifd::FileDialog::Instance().HasResult()) {
        auto result = ReaderWriter::read(ifd::FileDialog::Instance().GetResult().string());
        auto geometry = std::get<0>(result);
        if (geometry) {
          engine_.viewer()->scene()->addGeometry(geometry);
          engine_.viewer()->home();
        } else {
          std::cout << std::get<2>(result) << std::endl;
        }
      }
      ifd::FileDialog::Instance().Close();
    }
  }
}