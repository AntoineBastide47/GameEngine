//
// Scene.cpp
// Author: Antoine Bastide
// Date: 25.07.2025
//

#include "Engine2D/Scene.hpp"
#include "Engine/Macros/Profiling.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/SceneManager.hpp"
#include "Engine2D/Physics/Physics2D.hpp"
#include "Engine2D/Rendering/Camera2D.hpp"
#include "Engine2D/Rendering/Renderer2D.hpp"

namespace Engine2D {
  Scene::Scene()
    : cameraComponent(nullptr), initialized(false), renderingSystem() {}

  Rendering::Camera2D *Scene::MainCamera() {
    return SceneManager::ActiveScene()->cameraComponent;
  }

  void Scene::syncEntities() {
    for (auto &entity: entitiesToAdd) {
      entity->initialize();
      entities.emplace_back(std::move(entity));
    }
    entitiesToAdd.clear();

    #if MULTI_THREAD
    for (auto entity: entitiesToDestroy) {
      entity->free();
      std::erase_if(
        entities, [entity](const std::unique_ptr<Entity2D> &ent) {
          return ent.get() == entity;
        }
      );
    }
    entitiesToDestroy.clear();
    #endif
    for (auto entity: entitiesToRemove) {
      entity->destroy();
      #if !MULTI_THREAD
      entity->free();
      std::erase_if(
        entities, [entity](const std::unique_ptr<Entity2D> &ent) {
          return ent.get() == entity;
        }
      );
      #endif
    }
    entitiesToRemove.clear();
  }

  void Scene::removeEntity(Entity2D *entity) {
    if (entity && Game2D::instance && !entities.empty())
      entitiesToRemove.insert(entity);
  }

  void Scene::initialize() {
    if (initialized)
      return;

    initialized = true;
    const auto mainCam = Entity2D::Instantiate("Camera");
    if (!mainCam) {
      std::cerr << "ERROR: Failed to create main camera" << std::endl;
      Game2D::instance->quit();
      exit(EXIT_FAILURE);
    }
    cameraComponent = mainCam->AddComponent<Rendering::Camera2D>(
      -static_cast<float>(Game2D::instance->width / 2) * Game2D::instance->screenScaleFactor,
      static_cast<float>(Game2D::instance->width / 2) * Game2D::instance->screenScaleFactor,
      -static_cast<float>(Game2D::instance->height / 2) * Game2D::instance->screenScaleFactor,
      static_cast<float>(Game2D::instance->height / 2) * Game2D::instance->screenScaleFactor,
      -32768.0f, 32768.0f // int16_t range
    );
  }

  void Scene::update() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    for (const auto &entity: entities) {
      if (entity->IsActive()) {
        for (const auto &behaviour: entity->behaviours)
          if (behaviour->IsActive())
            behaviour->OnUpdate();
      }
      #if MULTI_THREAD
      else if (entity->destroyed) {
        entity->timeToLive -= Game2D::DeltaTime();
        if (entity->timeToLive <= 0.0f)
          entitiesToDestroy.insert(entity.get());
      }
      #endif
    }
  }

  void Scene::fixedUpdate() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    Game2D::instance->physicsAccumulator += Game2D::DeltaTime();
    const float fixedDeltaTime = Engine::Settings::Physics::GetFixedDeltaTime();
    while (Game2D::instance->physicsAccumulator >= fixedDeltaTime) {
      for (const auto &entity: entities)
        if (entity->IsActive())
          for (const auto &behaviour: entity->behaviours)
            if (behaviour->IsActive())
              behaviour->OnFixedUpdate();
      physicsSystem.step();
      Game2D::instance->physicsAccumulator -= fixedDeltaTime;
    }
  }

  void Scene::animate() const {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    animationSystem.update();
  }

  void Scene::render() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    // Make sure there is something to render
    if (!entities.empty()) {
      cameraComponent->updateCamera();
      renderingSystem.render();

      // Prepare the next frame
      glfwSwapBuffers(Game2D::instance->window);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
  }

  void Scene::destroy() {
    // Remove all the entities from the game
    entitiesToAdd.clear();
    syncEntities();
    for (const auto &entity: entities) {
      entity->destroy();
      entity->free();
    }
    entities.clear();
  }
}
