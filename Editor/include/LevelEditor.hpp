//
// LevelEditor.hpp
// Author: Antoine Bastide
// Date: 07.08.2025
//

#ifndef LEVEL_EDITOR_HPP
#define LEVEL_EDITOR_HPP

#include <memory>
#include <string>

#include "ProjectLoader.hpp"
#include "Engine2D/Game2D.hpp"

namespace Engine2D {
  class Game2D;
}

namespace Editor {
  class LevelEditor final {
    friend class Window;
    public:
      /// Renders the level editor
      static void Render();

      /// @returns True if the viewport is currently focused, False if not
      static bool IsViewportFocused();
      /// @returns True if the viewport is currently hovered, False if not
      static bool IsViewportHovered();
    private:
      inline static uint32_t rendererID = 0, colorAttachment = 0, depthAttachment = 0;
      inline static uint32_t highResRendererID = 0, highResColorAttachment = 0, highResDepthAttachment = 0;

      struct FrameBufferData {
        uint32_t width, height;
        static constexpr float supersampleFactor = 2.0f;
      };

      inline static FrameBufferData frameBufferData{800, 600};
      inline static ProjectLoader projectLoader;
      inline static std::unique_ptr<Engine2D::Game2D> loadedGame;
      inline static bool viewportFocused = false;
      inline static bool viewportHovered = false;

      inline static bool showLoadProjectDialog = false;
      inline static char projectPathBuffer[512] = "";

      /// Loads a project to be rendered in the level editor
      /// @param projectPath The path of the project to load
      static bool loadProject(const std::string &projectPath);
      /// Unloads the currently loaded project
      static void unloadProject();

      /// Renders the menu bar of the level editor
      static void renderMenuBar();
      /// Renders the viewport of the level editor
      static void renderViewport();

      /// Create's a framebuffer for the scene based on the given data
      static void createSceneFrameBuffer(const FrameBufferData &data);
      /// Bind's the scene's framebuffer
      static void bindSceneFrameBuffer();
      /// Unbind's the scene's framebuffer
      static void unbindSceneFrameBuffer();
      /// Resize's the scene's framebuffer to the given proportions
      static void resizeSceneFrameBuffer(uint32_t width, uint32_t height);

      /// Deletes all the framebuffers and images
      static void cleanup();
  };
} // Editor

#endif //LEVEL_EDITOR_HPP
