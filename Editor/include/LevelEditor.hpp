//
// LevelEditor.hpp
// Author: Antoine Bastide
// Date: 07.08.2025
//

#ifndef LEVEL_EDITOR_HPP
#define LEVEL_EDITOR_HPP

#include <memory>
#include <string>

#include "Engine2D/Game2D.hpp"

namespace Engine2D {
  class Game2D;
}

namespace Editor {
  class LevelEditor final {
    friend class Window;
    friend class SceneViewport;
    public:
      /// Renders the level editor
      static void Render();
    private:
      inline static std::unique_ptr<Engine2D::Game2D> loadedGame;

      inline static bool showLoadProjectDialog = false;
      inline static char projectPathBuffer[512] = "";

      /// Initializes the level editor
      static void initialize();

      /// Loads a project to be rendered in the level editor
      /// @param projectPath The path of the project to load
      static bool loadProject(const std::string &projectPath);
      /// Unloads the currently loaded project
      static void unloadProject();

      /// Renders the menu bar of the level editor
      static void renderMenuBar();
      /// Renders all the panels
      static void renderPanels();

      /// Deletes all the framebuffers and images
      static void cleanup();
  };
} // Editor

#endif //LEVEL_EDITOR_HPP
