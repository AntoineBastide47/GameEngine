//
// SceneManager.cpp
// Author: Antoine Bastide
// Date: 25.07.2025
//

#include <ranges>

#include "Engine2D/SceneManagement/SceneManager.hpp"
#include "Engine/Log.hpp"
#include "Engine/Reflection/Deserializer.hpp"
#include "Engine/Reflection/Serializer.hpp"

namespace Engine2D {
  Scene *SceneManager::CreateScene(const std::string &name) {
    if (scenes.contains(name))
      return Engine::Log::Error("Scene with name: " + name + " already exists!");

    const auto ptr = new Scene(name);
    scenes.emplace(name, std::unique_ptr<Scene>(ptr));
    return ptr;
  }

  void SceneManager::LoadScene(const std::string &name, const std::string &path) {
    auto loadedScene = Engine::Reflection::Deserializer::FromJsonFromFile<std::unique_ptr<Scene>>(path);

    const bool isActiveScene = activeScene && activeScene->name == name;

    if (scenes.contains(name))
      DestroyScene(name);

    loadedScene->name = name;
    loadedScene->loaded = true;
    scenes.emplace(name, std::move(loadedScene));

    if (isActiveScene)
      SetActiveScene(name);
  }

  void SceneManager::SaveScene(
    const std::string &name, const std::string &path, const Engine::Reflection::Format format, const bool prettyPrint,
    const char indentChar
  ) {
    if (scenes.contains(name)) {
      if (format == Engine::Reflection::Format::JSON)
        Engine::Reflection::Serializer::ToJsonToFile(scenes.at(name), path, prettyPrint, indentChar);
    }
  }

  void SceneManager::SaveScene(
    const Scene *scene, const std::string &path, const Engine::Reflection::Format format, const bool prettyPrint,
    const char indentChar
  ) {
    for (const auto &s: scenes | std::views::values)
      if (s.get() == scene)
        if (format == Engine::Reflection::Format::JSON)
          Engine::Reflection::Serializer::ToJsonToFile(s, path, prettyPrint, indentChar);
  }

  void SceneManager::SaveActiveScene(
    const std::string &path, const Engine::Reflection::Format format, const bool prettyPrint, const char indentChar
  ) {
    SaveScene(activeScene, path, format, prettyPrint, indentChar);
  }

  void SceneManager::UnloadScene(const std::string &name) {
    if (scenes.contains(name))
      scenes.at(name)->destroy();
  }

  void SceneManager::DestroyScene(const std::string &name) {
    if (scenes.contains(name)) {
      if (scenes.at(name).get() == activeScene)
        activeScene = nullptr;
      scenes.erase(name);
    }
  }

  void SceneManager::SetActiveScene(const std::string &name) {
    if (scenes.contains(name)) {
      activeScene = scenes.at(name).get();
      if (!activeScene->loaded)
        activeScene->initialize();
    } else
      activeScene = nullptr;
  }

  Scene *SceneManager::ActiveScene() {
    return activeScene;
  }
} // Engine2D
