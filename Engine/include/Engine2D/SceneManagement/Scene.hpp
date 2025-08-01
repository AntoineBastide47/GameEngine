//
// Scene.hpp
// Author: Antoine Bastide
// Date: 25.07.2025
//

#ifndef SCENE_HPP
#define SCENE_HPP

#include <unordered_set>

#include "Engine2D/Entity2D.hpp"
#include "Engine2D/Animation/AnimationSystem.hpp"
#include "Engine2D/ParticleSystem/ParticleSystemRegistry2D.hpp"
#include "Engine2D/Physics/Physics2D.hpp"
#include "Engine2D/Rendering/Renderer2D.hpp"
#include "Scene.gen.hpp"
#include "SceneResources.hpp"

namespace Engine {
  class Settings;
  class ResourceManager;
}

namespace Engine2D {
  class ParticleSystemRegistry2D;

  namespace Rendering {
    class Camera2D;
    class Renderer2D;
    class SpriteRenderer;
  }

  namespace Animation {
    class Animator2D;
  }

  namespace Physics {
    class Collider2D;
  }

  class Scene final : public Engine::Reflection::Reflectable {
    SERIALIZE_SCENE
      friend class Game2D;
      friend class Entity2D;
      friend class Transform2D;
      friend class SceneManager;
      friend class Engine::Settings;
      friend class ParticleSystem2D;
      friend class Physics::Collider2D;
      friend class Rendering::Camera2D;
      friend class Animation::Animator2D;
      friend class Rendering::Renderer2D;
      friend class Engine::ResourceManager;
      friend class ParticleSystemRegistry2D;
      friend class Rendering::SpriteRenderer;
    public:
      ~Scene() override;

      /// @returns The name of this scene
      [[nodiscard]] const std::string &Name();

      /// Changes the main camera to the given camera component
      void SetMainCamera(Rendering::Camera2D *camera);
      /// @returns A pointer to the main camera of this scene
      [[nodiscard]] Rendering::Camera2D *MainCamera() const;

      /// @returns The entity with the given name if it was found, nullptr if not
      Entity2D *Find(const std::string &name) const;

      template<typename T> requires std::is_base_of_v<Component2D, T>
      /// @returns The first entity with the given component, nullptr if no entity is found
      T *FindObjectOfType() const {
        if constexpr (std::is_same_v<Transform2D, T>)
          return entities.front()->Transform();

        for (const auto &entity: entities)
          if (T *component = nullptr; entity->TryGetComponent<T>(&component))
            return component;

        return nullptr;
      }

      template<typename T> requires std::is_base_of_v<Component2D, T>
      /// @returns All the entities with the given component, nullptr if no entities are found
      std::vector<T *> FindObjectsOfType() const {
        std::vector<T *> res;
        for (const auto &entity: entities)
          if (T *component = nullptr; entity->TryGetComponent<T>(&component))
            res.emplace_back(component);

        return res;
      }

      void OnDeserialize(Engine::Reflection::Format format, const Engine::JSON &json) override;
    private:
      /// All the resources in this scene
      ENGINE_SERIALIZE SceneResources resources;
      /// All the entities currently in the game
      ENGINE_SERIALIZE std::vector<std::unique_ptr<Entity2D>> entities;
      /// The name of this scene
      std::string name;
      /// The main camera of the game
      Rendering::Camera2D *cameraComponent;
      /// If the scene has been initialized
      bool initialized;
      /// If the scene has been loaded into memory
      bool loaded;
      /// Entities scheduled to be added to the game
      std::vector<std::unique_ptr<Entity2D>> entitiesToAdd;
      /// Entities scheduled to be removed from the game
      std::unordered_set<Entity2D *> entitiesToRemove;
      #if MULTI_THREAD
      /// Entities that should be removed from memory
      std::unordered_set<Entity2D *> entitiesToDestroy;
      #endif

      Physics::Physics2D physicsSystem;
      Rendering::Renderer2D renderingSystem;
      Animation::AnimationSystem animationSystem;
      ParticleSystemRegistry2D particleSystemRegistry;

      Scene();
      explicit Scene(const std::string &name);

      /// Adds and removes all the pending entities
      void syncEntities();
      /// Add's an entity to the game
      void addEntity(std::unique_ptr<Entity2D> entity);
      /// Removes an entity from the game
      void removeEntity(Entity2D *entity);

      /// Initializes the scene
      void initialize();
      /// Updates all the entities in this scene
      #if MULTI_THREAD
      void update();
      #else
      void update() const;
      #endif
      /// Call the physics step for all the entities in this scene
      void fixedUpdate();
      /// Animates all entities
      void animate() const;
      /// Renders all the entities in this scene
      void render();
      /// Destroys this scene
      void destroy();
  };
}

#endif //SCENE_HPP
