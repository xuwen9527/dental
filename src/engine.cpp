#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_glfw.h"
#include "external/imgui/imgui_impl_opengl3.h"
#include "external/ifd/ImFileDialog.h"
#include <iostream>
#include <fstream>
#include <config.h>
#include <engine.h>
#include <ui/menubar.h>
#include <ui/undercut.h>
#include <ui/project.h>
#include <ui/preview.h>
#include <render_visitor.h>

namespace Dental {
  Engine::Engine() :
    window_(nullptr),
    viewer_(std::make_shared<Viewer>()) {
    uiviews_.emplace_back(std::make_shared<UI::MenuBar>(*this));
    uiviews_.emplace_back(std::make_shared<UI::UnderCut>(*this));
    // uiviews_.emplace_back(std::make_shared<UI::Preview>(*this));
  }

  Engine::~Engine() {

  }

  void Engine::setupFonts() {
    {
      ifd::FileDialog::Instance().CreateTexture = [](uint8_t* data, int w, int h, char fmt) -> void* {
        GLuint tex;

        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, (fmt == 0) ? GL_RGBA : GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        return (void*)(size_t)tex;
      };
      ifd::FileDialog::Instance().DeleteTexture = [](void* tex) {
        GLuint texID = (GLuint)((uintptr_t)tex);
        glDeleteTextures(1, &texID);
      };
      ifd::FileDialog::Instance().SetZoom(1.f);
    }

    glfwSetErrorCallback([](int error, const char* description) {
      std::cout << "Glfw Error " << error << " " << description;
    });

    {
      static std::string defaultFontPath = "msyh.ttc";
      static float defaultFontSize = 20.f;

      auto font = ImGui::GetIO().Fonts->AddFontFromFileTTF(
        defaultFontPath.c_str(), defaultFontSize, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesChineseFull());

      if (font) {
        ImGui::GetIO().FontDefault = font;
      }
    }

    // {
    //   static std::string fa_solid_file_name = "../fonts/fa-solid-900.ttf";
    //   static std::string fa_regular_file_name = "../fonts/fa-regular-400.ttf";
    //   static float iconFontSize = 24.f;
    //   static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    //   ImFontConfig fa_solid_config;
    //   fa_solid_config.MergeMode = true;
    //   fa_solid_config.PixelSnapH = true;

    //   ImFontConfig fa_regular_config;
    //   fa_regular_config.MergeMode = true;
    //   fa_regular_config.PixelSnapH = true;
    //   ImGui::GetIO().Fonts->AddFontFromFileTTF(fa_solid_file_name.c_str(), iconFontSize, &fa_solid_config, icons_ranges);
    //   ImGui::GetIO().Fonts->AddFontFromFileTTF(fa_regular_file_name.c_str(), iconFontSize, &fa_regular_config, icons_ranges);
    // }
  }

  void Engine::setup() {      
    glfwSetErrorCallback([](int error, const char* description) {
      std::cout << "Glfw Error " << error << " " << description;
    });

    if (!glfwInit()) {
      std::cout << "glfwInit is error." << std::endl;
      return;
    }

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_MAXIMIZED, true);
    glfwWindowHint(GLFW_DECORATED, true);
    glfwWindowHint(GLFW_RESIZABLE, true);
    glfwWindowHint(GLFW_REFRESH_RATE, 60);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    window_ = glfwCreateWindow(800, 600, "Dental", nullptr, nullptr);
    if (!window_) {
      std::cout << "failed to create GLFW window" << std::endl;
      return;
    }

    glfwMakeContextCurrent(window_);

    if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)) {
      std::cout << "failed to load glad" << std::endl;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(8, 8);
    style.WindowBorderSize = 1;
    style.WindowMenuButtonPosition = ImGuiDir_None;
    style.FrameBorderSize = 1;
    style.FrameRounding = 4;

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    // io.ConfigDockingAlwaysTabBar = true;
    io.FontAllowUserScaling = true;

    setupFonts();

    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init();

    int width, height;
    glfwGetWindowSize(window_, &width, &height);
    viewer_->scene()->resize(width, height);

    glfwSetWindowUserPointer(window_, this);
    glfwSetWindowSizeCallback(window_, [](GLFWwindow* window, int width, int height) {
      Engine* engine = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
      engine->viewer()->scene()->resize(width, height);
    });

    glfwSetWindowRefreshCallback(window_, [](GLFWwindow* window) {
      Engine* engine = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
      engine->viewer()->events().redraw();
    });

    glfwSetKeyCallback(window_, ImGui_ImplGlfw_KeyCallback);
    glfwSetCharCallback(window_, ImGui_ImplGlfw_CharCallback);
    glfwSetMouseButtonCallback(window_, ImGui_ImplGlfw_MouseButtonCallback);
   
    glfwSetScrollCallback(window_, [](GLFWwindow* window, double xoffset, double yoffset){
      ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);

      Engine* engine = reinterpret_cast<Engine*>(glfwGetWindowUserPointer(window));
      Event event(yoffset > 0.0 ? Event::POINTER_SCROLL_UP : Event::POINTER_SCROLL_DOWN);
      engine->viewer()->events().push(event);
    });
  }

  void Engine::render() {
    Event event;

    ImGuiIO& io = ImGui::GetIO();
    if (ImGui::IsMouseDragging(ImGuiPopupFlags_MouseButtonLeft)) {
      event.type(Event::POINTER_MOVE);
      event.button(Event::LEFT_BUTTON);
    } else if (ImGui::IsMouseDragging(ImGuiPopupFlags_MouseButtonRight)) {
      event.type(Event::POINTER_MOVE);
      event.button(Event::RIGHT_BUTTON);
    } else if (ImGui::IsMouseDragging(ImGuiPopupFlags_MouseButtonMiddle)) {
      event.type(Event::POINTER_MOVE);
      event.button(Event::MIDDLE_BUTTON);
    } else if (ImGui::IsMouseDown(ImGuiPopupFlags_MouseButtonLeft)) {
      event.type(Event::POINTER_PRESS);
      event.button(Event::LEFT_BUTTON);
    } else if (ImGui::IsMouseDown(ImGuiPopupFlags_MouseButtonRight)) {
      event.type(Event::POINTER_PRESS);
      event.button(Event::RIGHT_BUTTON);
    } else if (ImGui::IsMouseDown(ImGuiPopupFlags_MouseButtonMiddle)) {
      event.type(Event::POINTER_PRESS);
      event.button(Event::MIDDLE_BUTTON);
    } else if (ImGui::IsMouseReleased(ImGuiPopupFlags_MouseButtonLeft)) {
      event.type(Event::POINTER_RELEASE);
      event.button(Event::LEFT_BUTTON);
    } else if (ImGui::IsMouseReleased(ImGuiPopupFlags_MouseButtonRight)) {
      event.type(Event::POINTER_RELEASE);
      event.button(Event::RIGHT_BUTTON);
    } else if (ImGui::IsMouseReleased(ImGuiPopupFlags_MouseButtonMiddle)) {
      event.type(Event::POINTER_RELEASE);
      event.button(Event::MIDDLE_BUTTON);
    }

    if (event.type() & Event::POINTER) {
      event.firstPoint(io.MousePos.x, io.MousePos.y);
      viewer_->events().push(event);
    }

    viewer_->frame();
  }

  void Engine::run() {
    setup();

    while (!glfwWindowShouldClose(window_)) {
      glfwPollEvents();

      if (ImGui::HasEvent()) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClearColor(0.45f, 0.55f, 0.60f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        for (auto& uiview : uiviews_) {
          uiview->render();
        }

        render();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window_);
      }
    }

    ImGui_ImplGlfw_Shutdown();
    glfwDestroyWindow(window_);
    glfwTerminate();
  }
}