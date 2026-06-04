#include "gui.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"

void guiInstance::init(GLFWwindow *window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");
}

void guiInstance::draw() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImGuiID dockId = ImGui::DockSpaceOverViewport(0, viewport);

  static bool layoutBuilt = false;
  if (!layoutBuilt) {
    layoutBuilt = true;

    ImGui::DockBuilderRemoveNode(dockId);
    ImGui::DockBuilderAddNode(dockId, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockId, viewport->Size);

    ImGuiID right;
    ImGuiID left = ImGui::DockBuilderSplitNode(dockId, ImGuiDir_Left, 0.33f,
                                               nullptr, &right);
    ImGuiID bottomRight;
    ImGuiID topRight = ImGui::DockBuilderSplitNode(right, ImGuiDir_Up, 0.5f,
                                                   nullptr, &bottomRight);

    ImGui::DockBuilderDockWindow("Options", left);
    ImGui::DockBuilderDockWindow("Assets", topRight);
    ImGui::DockBuilderDockWindow("Render", bottomRight);
    ImGui::DockBuilderFinish(dockId);
  }

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("View")) {
      ImGui::MenuItem("Options", NULL, &showOptions);
      ImGui::MenuItem("Assets", NULL, &showAssets);
      ImGui::MenuItem("Render", NULL, &shoFlowEngineder);
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  if (showOptions)
    optionsPanel();
  if (showAssets)
    assetsPanel();
  if (shoFlowEngineder)
    renderPanel();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void guiInstance::assetsPanel() {
  ImGui::Begin("Assets");
  ImGui::End();
}

void guiInstance::renderPanel() {
  ImGui::Begin("Render");
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
