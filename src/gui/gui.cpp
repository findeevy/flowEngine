#include "gui.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

void guiInstance::init(GLFWwindow *window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();

  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 430");
}

void guiInstance::draw() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("View")) {
      ImGui::MenuItem("Options", NULL, &showOptions);
      ImGui::MenuItem("Assets", NULL, &showAssets);
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  if (showOptions) {
    optionsPanel();
  }
  if (showAssets) {
    assetsPanel();
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void guiInstance::assetsPanel() {
  ImGui::Begin("Assets");
  ImGui::End();
}

void guiInstance::optionsPanel() {
  ImGui::Begin("Options");
  ImGui::End();
}

std::vector<Event> guiInstance::poll() {
  std::vector<Event> out = std::move(guiQueue);
  guiQueue.clear();
  return out;
}

void guiInstance::shutdown() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
