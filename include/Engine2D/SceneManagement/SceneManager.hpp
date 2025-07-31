//
// SceneManager.hpp
// Author: Antoine Bastide
// Date: 25.07.2025
//

#ifndef SCENE_MANAGER_HPP
#define SCENE_MANAGER_HPP

#include <string>
#include <unordered_map>

#include "Scene.hpp"

namespace Engine {
  class ResourceManager;
}

namespace Engine2D {
  class SceneManager final {
    friend class Game2D;
    friend class Engine::ResourceManager;
    public:
      /// Create's a new scene with the given name
      /// @param name The name of the new scene
      /// @note Returns null if a scene with the given name already exists
      /// @returns A pointer to the newly created scene
      static Scene *CreateScene(const std::string &name);

      /// Load's the scene with the given name into memory
      /// @param name The name of the scene to load
      /// @param path The file in which the scene to load is stored
      /// @note If a scene with the given name already exists, it will be overridden
      static void LoadScene(const std::string &name, const std::string &path);

      /// Save's the scene with the given name into the given file
      /// @param name The name of the scene to save
      /// @param path The file in which to save the scene
      /// @param format The format in which to save this scene
      /// @param prettyPrint Whether to pretty print the data or not
      /// @param indentChar The indent char to use
      static void SaveScene(
        const std::string &name, const std::string &path, Engine::Reflection::Format format, bool prettyPrint = false,
        char indentChar = ' '
      );

      /// Save's the given scene into the given file
      /// @param scene The scene to save
      /// @param path The file in which to save the scene
      /// @param format The format in which to save this scene
      /// @param prettyPrint Whether to pretty print the data or not
      /// @param indentChar The indent char to use
      static void SaveScene(
        const Scene *scene, const std::string &path, Engine::Reflection::Format format, bool prettyPrint = false,
        char indentChar = ' '
      );

      /// Save's the currently active scene into the given file
      /// @param path The file in which to save the scene
      /// @param format The format in which to save this scene
      /// @param prettyPrint Whether to pretty print the data or not
      /// @param indentChar The indent char to use
      static void SaveActiveScene(
        const std::string &path, Engine::Reflection::Format format, bool prettyPrint = false, char indentChar = ' '
      );

      /// Unload's the scene with the given name and remove's all it's entities and their components from memory
      /// @param name The name of the scene to destroy
      static void UnloadScene(const std::string &name);

      /// Remove's the scene with the given name, all it's entities and their components from memory
      /// @param name The name of the scene to destroy
      static void DestroyScene(const std::string &name);

      /// Set's the scene with the given scene as the active scene
      /// @param name The name of the scene to set active
      /// @note If the scene is not loaded, it will be loaded
      /// @note If no scene with the given name exist, the active scene will be set to null
      static void SetActiveScene(const std::string &name);

      /// @returns A pointer to the currently loaded scene
      static Scene *ActiveScene();
    private:
      /// All the created or loaded scenes in the game
      inline static std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;
      /// The scene currently displayed
      inline static Scene *activeScene;
  };
} // Engine2D

#endif //SCENE_MANAGER_HPP
