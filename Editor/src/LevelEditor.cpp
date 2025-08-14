//
// LevelEditor.cpp
// Author: Antoine Bastide
// Date: 07.08.2025
//

#include <filesystem>
#include <imgui.h>
#include <iostream>

#include "LevelEditor.hpp"
#include "ProjectLoader.hpp"
#include "Window.hpp"
#include "Engine/Input/Keyboard.hpp"
#include "Engine/Macros/Assert.hpp"
#include "Engine2D/Game2D.hpp"

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

    renderViewport();
  }

  void LevelEditor::renderViewport() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoCollapse);
    const ImVec2 viewportSize = ImGui::GetContentRegionAvail();

    // Ensure minimum viewport size
    const float width = std::max(viewportSize.x, 800.0f);
    const float height = std::max(viewportSize.y, 600.0f);

    if (frameBufferData.width != width || frameBufferData.height != height)
      resizeSceneFrameBuffer(static_cast<uint32_t>(width), static_cast<uint32_t>(height));

    bindSceneFrameBuffer();
    if (projectLoader.IsProjectLoaded() && loadedGame) {
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      loadedGame->renderFrame();
    } else if (projectLoader.IsProjectLoaded()) {
      glClearColor(0.3f, 0.3f, 0.1f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    } else {
      glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    unbindSceneFrameBuffer();

    // Cast texture ID for ImGui (flipped on Y-axis)
    ImGui::Image(colorAttachment, ImVec2(width, height), ImVec2(0, 1), ImVec2(1, 0));

    // Update viewport state for input processing
    viewportFocused = ImGui::IsWindowFocused();
    viewportHovered = ImGui::IsItemHovered();
    ImGui::End();
    ImGui::PopStyleVar();
  }

  void LevelEditor::createSceneFrameBuffer(const FrameBufferData &data) {
    if (rendererID)
      cleanup();

    const uint32_t highResWidth = static_cast<uint32_t>(data.width * data.supersampleFactor);
    const uint32_t highResHeight = static_cast<uint32_t>(data.height * data.supersampleFactor);

    // Create high-resolution framebuffer for supersampling
    glGenFramebuffers(1, &highResRendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, highResRendererID);

    glGenTextures(1, &highResColorAttachment);
    glBindTexture(GL_TEXTURE_2D, highResColorAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, highResWidth, highResHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, highResColorAttachment, 0);

    glGenTextures(1, &highResDepthAttachment);
    glBindTexture(GL_TEXTURE_2D, highResDepthAttachment);
    glTexImage2D(
      GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, highResWidth, highResHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8,
      nullptr
    );
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, highResDepthAttachment, 0);

    ENGINE_ASSERT(
      glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "High-res framebuffer incomplete"
    );

    // Create display-resolution framebuffer
    glGenFramebuffers(1, &rendererID);
    glBindFramebuffer(GL_FRAMEBUFFER, rendererID);

    glGenTextures(1, &colorAttachment);
    glBindTexture(GL_TEXTURE_2D, colorAttachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, data.width, data.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);

    glGenTextures(1, &depthAttachment);
    glBindTexture(GL_TEXTURE_2D, depthAttachment);
    glTexImage2D(
      GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, data.width, data.height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, nullptr
    );
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);

    ENGINE_ASSERT(
      glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Display framebuffer incomplete"
    );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void LevelEditor::bindSceneFrameBuffer() {
    const uint32_t highResWidth = static_cast<uint32_t>(frameBufferData.width * frameBufferData.supersampleFactor);
    const uint32_t highResHeight = static_cast<uint32_t>(frameBufferData.height * frameBufferData.supersampleFactor);

    glBindFramebuffer(GL_FRAMEBUFFER, highResRendererID);
    glViewport(0, 0, highResWidth, highResHeight);
  }

  void LevelEditor::unbindSceneFrameBuffer() {
    // Blit high-resolution framebuffer to display-resolution framebuffer with linear filtering
    glBindFramebuffer(GL_READ_FRAMEBUFFER, highResRendererID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rendererID);

    const uint32_t highResWidth = static_cast<uint32_t>(frameBufferData.width * frameBufferData.supersampleFactor);
    const uint32_t highResHeight = static_cast<uint32_t>(frameBufferData.height * frameBufferData.supersampleFactor);

    glBlitFramebuffer(
      0, 0, highResWidth, highResHeight,
      0, 0, frameBufferData.width, frameBufferData.height,
      GL_COLOR_BUFFER_BIT, GL_LINEAR
    );

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void LevelEditor::resizeSceneFrameBuffer(uint32_t width, uint32_t height) {
    frameBufferData.width = width;
    frameBufferData.height = height;
    createSceneFrameBuffer(frameBufferData);

    if (loadedGame)
      loadedGame->framebuffer_size_callback(nullptr, width, height);
  }

  bool LevelEditor::loadProject(const std::string &projectPath) {
    // Unload any existing project first
    unloadProject();

    // Use ProjectLoader to build and load the project
    if (!projectLoader.LoadProject(projectPath)) {
      std::cerr << "Failed to load project with ProjectLoader" << std::endl;
      return false;
    }

    // Get the plugin and create the game instance
    IGamePlugin *plugin = projectLoader.GetPlugin();
    if (!plugin) {
      std::cerr << "Plugin not available after project load" << std::endl;
      return false;
    }

    // Create the game instance in headless mode
    loadedGame = plugin->CreateGame(frameBufferData.width, frameBufferData.height);
    if (!loadedGame) {
      std::cerr << "Failed to create game instance from plugin" << std::endl;
      return false;
    }

    // Set up resources for the game
    plugin->SetupResources(loadedGame.get());
    loadedGame->initializeGamePipeline(Window::instance->window);
    loadedGame->setAsHeadless();
    return true;
  }

  void LevelEditor::unloadProject() {
    // Reset the game instance first
    if (loadedGame) {
      loadedGame.reset();
    }

    // Unload the project through ProjectLoader
    projectLoader.UnloadProject();
  }

  void LevelEditor::renderMenuBar() {
    if (ImGui::BeginMenuBar()) {
      if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("Load Project..."))
          showLoadProjectDialog = true;

        if (ImGui::MenuItem("Unload Project", nullptr, false, projectLoader.IsProjectLoaded()))
          unloadProject();

        ImGui::Separator();

        if (ImGui::MenuItem("Exit"))
          Window::Quit();

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

      if (ImGui::Button("Load", ImVec2(80, 0))) {
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
      if (ImGui::Button("Cancel", ImVec2(80, 0))) {
        ImGui::CloseCurrentPopup();
      }

      ImGui::EndPopup();
    }
  }

  void LevelEditor::cleanup() {
    // Clean up display resolution framebuffer
    if (rendererID) {
      glDeleteFramebuffers(1, &rendererID);
      rendererID = 0;
    }
    if (colorAttachment) {
      glDeleteTextures(1, &colorAttachment);
      colorAttachment = 0;
    }
    if (depthAttachment) {
      glDeleteTextures(1, &depthAttachment);
      depthAttachment = 0;
    }

    // Clean up high resolution framebuffer
    if (highResRendererID) {
      glDeleteFramebuffers(1, &highResRendererID);
      highResRendererID = 0;
    }
    if (highResColorAttachment) {
      glDeleteTextures(1, &highResColorAttachment);
      highResColorAttachment = 0;
    }
    if (highResDepthAttachment) {
      glDeleteTextures(1, &highResDepthAttachment);
      highResDepthAttachment = 0;
    }
  }

  bool LevelEditor::IsViewportFocused() {
    return viewportFocused;
  }

  bool LevelEditor::IsViewportHovered() {
    return viewportHovered;
  }
} // Editor
