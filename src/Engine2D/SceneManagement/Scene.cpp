//
// Scene.cpp
// Author: Antoine Bastide
// Date: 25.07.2025
//

#include "Engine2D/SceneManagement/Scene.hpp"
#include "Engine/Macros/Profiling.hpp"
#include "Engine2D/Component2D.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/Physics/Physics2D.hpp"
#include "Engine2D/Rendering/Camera2D.hpp"
#include "Engine2D/Rendering/Renderer2D.hpp"
#include "Engine2D/SceneManagement/SceneManager.hpp"

namespace Engine2D {
  Scene::Scene()
    : Scene("default") {}

  Scene::~Scene() {
    destroy();
  }

  Scene::Scene(const std::string &name)
    : name(name), cameraComponent(nullptr), initialized(false), loaded(false), renderingSystem() {
    renderingSystem.scene = this;
    particleSystemRegistry.scene = this;
  }

  const std::string &Scene::Name() {
    return name;
  }

  void Scene::SetMainCamera(Rendering::Camera2D *camera) {
    cameraComponent = camera;
  }

  Rendering::Camera2D *Scene::MainCamera() const {
    return cameraComponent;
  }

  Entity2D *Scene::Find(const std::string &name) const {
    for (const auto &entity: entitiesToAdd)
      if (entity->name == name)
        return entity.get();
    for (const auto &entity: entities)
      if (entity->name == name)
        return entity.get();
    return nullptr;
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

  void Scene::addEntity(std::unique_ptr<Entity2D> entity) {
    if (!entity)
      return;
    entity->scene = this;
    entitiesToAdd.emplace_back(std::move(entity));
  }

  void Scene::removeEntity(Entity2D *entity) {
    if (!entity)
      return;
    entitiesToRemove.insert(entity);
  }

  void Scene::initialize() {
    if (initialized || loaded)
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

  #if MULTI_THREAD
  void Scene::update() {
    #else
  void Scene::update() const {
    #endif
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    for (const auto &entity: entities) {
      if (entity->IsActive()) {
        for (const auto &behaviour: entity->behavioursToInitialize) {
          behaviour->OnInitialize();
          behaviour->OnBindInput();
        }
        entity->behavioursToInitialize.clear();
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
      SceneManager::ActiveScene()->MainCamera()->updateCamera();
      renderingSystem.render();

      // Prepare the next frame
      glfwSwapBuffers(Game2D::instance->window);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
  }

  void Scene::destroy() {
    // Remove all the entities from the game
    for (const auto &entity: entitiesToAdd) {
      entity->destroy();
      entity->free();
    }
    for (const auto &entity: entities) {
      entity->destroy();
      entity->free();
    }
    entities.clear();
    entitiesToAdd.clear();
  }

  void Scene::OnDeserialize(const Engine::Reflection::Format format, const Engine::JSON &json) {
    std::erase(entities, nullptr);

    int i = 0;
    for (auto it = entities.begin(); it != entities.end();) {
      const auto entity = it->get();
      if (entity->destroyed) {
        entity->destroy();
        entity->free();
        it = entities.erase(it);
        continue;
      }

      const auto &entityData = json.At("entities").At(i++).At("data");
      const int index = static_cast<int>(entityData.At("transform").At("data").At("parent").GetNumber());
      const auto transform = entity->Transform();

      entity->scene = this;
      transform->entity = entity;
      transform->SetPositionRotationAndScale(transform->worldPosition, transform->worldRotation, transform->worldScale);
      transform->SetParent(index == -1 ? nullptr : (entities.begin() + index)->get());

      std::erase(entity->allComponents, nullptr);
      for (auto &component: entity->allComponents) {
        component->entity = entity;
        if (const auto behaviour = dynamic_cast<Behaviour *>(component.get()); behaviour) {
          behaviour->OnBindInput();
          entity->behaviours.emplace_back(behaviour);
        } else {
          component->forward();
          entity->components.emplace_back(component.get());
        }
      }

      int j = 0;
      for (const auto &component: entity->allComponents)
        component->OnDeserialize(format, entityData.At("allComponents").At(j++).At("data"));
      ++it;
    }

    cameraComponent = Find("Camera")->GetComponent<Rendering::Camera2D>();
    if (cameraComponent->followTargetIndex > -1)
      cameraComponent->followTarget = entities.at(cameraComponent->followTargetIndex).get();
  }
}
