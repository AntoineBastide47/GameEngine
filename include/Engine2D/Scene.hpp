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

namespace Engine {
  class Settings;
}

namespace Engine2D {
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
      friend class SceneManager;
      friend class Engine::Settings;
      friend class ParticleSystem2D;
      friend class Physics::Collider2D;
      friend class Animation::Animator2D;
      friend class Rendering::Renderer2D;
      friend class Rendering::SpriteRenderer;
    public:
      /// @returns A pointer to the main camera of this scene
      [[nodiscard]] static Rendering::Camera2D *MainCamera();
    private:
      /// The main camera of the game
      Rendering::Camera2D *cameraComponent;
      /// If the scene has been initialized
      bool initialized;
      /// All the entities currently in the game
      std::vector<std::unique_ptr<Entity2D>> entities;
      /// Entities scheduled to be added to the game
      std::vector<std::unique_ptr<Entity2D>> entitiesToAdd;
      /// Entities scheduled to be removed from the game
      std::unordered_set<Entity2D *> entitiesToRemove;
      #if MULTI_THREAD
      /// Entities that should be removed from memory
      std::unordered_set<Entity2D *> entitiesToDestroy;
      #endif

      Animation::AnimationSystem animationSystem;
      ParticleSystemRegistry2D particleSystemRegistry;
      Physics::Physics2D physicsSystem;
      Rendering::Renderer2D renderingSystem;

      Scene();

      /// Adds and removes all the pending entities
      void syncEntities();
      /// Removes an entity from the game
      void removeEntity(Entity2D *entity);

      /// Initializes the scene
      void initialize();
      /// Updates all the entities in this scene
      void update();
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
