//
// SceneManager.cpp
// Author: Antoine Bastide
// Date: 25.07.2025
//

#include "Engine2D/SceneManager.hpp"

namespace Engine2D {
  Scene *SceneManager::CreateScene(const std::string &name) {
    const auto ptr = new Scene();
    scenes.emplace(name, std::unique_ptr<Scene>(ptr));
    return ptr;
  }

  void SceneManager::UnloadScene(const std::string &name) {
    if (scenes.contains(name))
      scenes.at(name)->destroy();
  }

  void SceneManager::DestroyScene(const std::string &name) {
    if (scenes.contains(name)) {
      scenes.at(name)->destroy();
      scenes.erase(name);
    }
  }

  void SceneManager::SetActiveScene(const std::string &name) {
    if (scenes.contains(name)) {
      if (currentScene != scenes.at(name).get()) {
        currentScene = scenes.at(name).get();
        scenes.at(name)->initialize();
      }
    }
  }

  Scene *SceneManager::ActiveScene() {
    return currentScene;
  }
} // Engine2D
