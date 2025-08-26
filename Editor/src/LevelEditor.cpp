//
// LevelEditor.cpp
// Author: Antoine Bastide
// Date: 07.08.2025
//

#include <filesystem>
#include <imgui.h>
#include <iostream>

#include "LevelEditor.hpp"
#include "imgui_internal.h"
#include "ProjectLoader.hpp"
#include "Window.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/SceneManagement/SceneManager.hpp"
#include "Panels/EntityInspector.hpp"
#include "Panels/SceneHierarchy.hpp"
#include "Panels/SceneViewport.hpp"

namespace Editor {
  void LevelEditor::Render() {
    static bool opt_fullscreen = true;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
      const ImGuiViewport *viewport = ImGui::GetMainViewport();
      ImGui::SetNextWindowPos(viewport->WorkPos);
      ImGui::SetNextWindowSize(viewport->WorkSize);
      ImGui::SetNextWindowViewport(viewport->ID);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
      window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
          ImGuiWindowFlags_NoMove;
      window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
    // and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
      window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    bool show = true;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Level Editor", &show, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
      ImGui::PopStyleVar(2);

    if (const ImGuiIO &io = ImGui::GetIO(); io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
      const ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
      ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    renderMenuBar();
    ImGui::End();

    SceneHierarchy::SetContext(Engine2D::SceneManager::ActiveScene());

    renderPanels();
  }

  void LevelEditor::initialize() {
    SceneViewport::Initialize();
  }

  bool LevelEditor::loadProject(const std::string &projectPath) {
    // Unload any existing project first
    unloadProject();

    // Use ProjectLoader to build and load the project
    if (!ProjectLoader::LoadProject(projectPath)) {
      std::cerr << "Failed to load project with ProjectLoader" << std::endl;
      return false;
    }

    // Get the plugin and create the game instance
    IGamePlugin *plugin = ProjectLoader::GetPlugin();
    if (!plugin) {
      std::cerr << "Plugin not available after project load" << std::endl;
      return false;
    }

    // Create the game instance in headless mode
    loadedGame = plugin->CreateGame(800, 600);
    if (!loadedGame) {
      std::cerr << "Failed to create game instance from plugin" << std::endl;
      return false;
    }

    // Set up resources for the game
    plugin->SetupResources(loadedGame.get());
    loadedGame->initializeGamePipeline(Window::instance->window);
    loadedGame->setAsHeadless();
    SceneViewport::resizeSceneFrameBuffer(SceneViewport::frameBufferData.width, SceneViewport::frameBufferData.height);
    return true;
  }

  void LevelEditor::unloadProject() {
    // Reset the game instance first
    if (loadedGame) {
      loadedGame.reset();
    }

    // Unload the project through ProjectLoader
    ProjectLoader::UnloadProject();
  }

  void LevelEditor::renderMenuBar() {
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Load Project..."))
          showLoadProjectDialog = true;

        if (ImGui::MenuItem("Unload Project", nullptr, false, ProjectLoader::IsProjectLoaded()))
          unloadProject();

        ImGui::Separator();

        if (ImGui::MenuItem("Exit"))
          Window::Quit();

        ImGui::EndMenu();
      }

      if (ImGui::BeginMenu("Edit")) {
        const bool canUndo = History::CommandHistory::CanUndo();
        const bool canRedo = History::CommandHistory::CanRedo();

        if (ImGui::MenuItem((canUndo ? "Undo: " + History::CommandHistory::UndoCmdName() : "Undo").c_str(), "", false, canUndo))
          History::CommandHistory::Undo();
        if (ImGui::MenuItem((canRedo ? "Redo: " + History::CommandHistory::RedoCmdName() : "Redo").c_str(), "", false, canRedo))
          History::CommandHistory::Redo();

        ImGui::EndMenu();
      }

      ImGui::EndMenuBar();
    }

    // Load Project Dialog
    if (showLoadProjectDialog) {
      ImGui::OpenPopup("Load Project");
      showLoadProjectDialog = false;
    }

    // Center the popup
    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 120), ImGuiCond_Appearing);

    if (ImGui::BeginPopupModal("Load Project", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
      ImGui::Text("Enter the path to the game project:");
      ImGui::Spacing();

      ImGui::SetNextItemWidth(-1);
      ImGui::InputText("##ProjectPath", projectPathBuffer, sizeof(projectPathBuffer));
      ImGui::Spacing();

      if (ImGui::Button("Load", ImVec2(187.5f, 0))) {
        if (strlen(projectPathBuffer) > 0) {
          if (std::filesystem::exists(projectPathBuffer)) {
            if (!loadProject(projectPathBuffer)) {
              std::cout << "Failed to load project: " << projectPathBuffer << std::endl;
            }
          } else {
            std::cout << "Project path does not exist: " << projectPathBuffer << std::endl;
          }
        }
        ImGui::CloseCurrentPopup();
      }

      ImGui::SameLine();
      if (ImGui::Button("Cancel", ImVec2(187.5f, 0))) {
        ImGui::CloseCurrentPopup();
      }

      ImGui::EndPopup();
    }
  }

  void LevelEditor::renderPanels() {
    ImGuiWindowClass winClass;
    winClass.DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoWindowMenuButton;

    SceneHierarchy::Render(&winClass);
    SceneViewport::Render(&winClass);
    EntityInspector::Render(&winClass);
  }

  void LevelEditor::cleanup() {
    SceneViewport::Cleanup();
  }
} // Editor
