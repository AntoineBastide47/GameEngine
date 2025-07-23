//
// Entity2D.h
// Author: Antoine Bastide
// Date: 03/11/2024
//

#ifndef ENTITY2D_H
#define ENTITY2D_H

#include <iostream>
#include <string>
#include <vector>

#include "Engine/Macros/Profiling.hpp"
#include "Engine/Reflection/Reflectable.hpp"
#include "Engine2D/Behaviour.hpp"
#include "Engine2D/Component2D.hpp"
#include "Engine2D/Transform2D.hpp"
#include "Entity2D.gen.hpp"

namespace Engine2D {
  namespace Physics {
    class Physics2D;
  }

  namespace Rendering {
    class Renderable2D;
  }

  /// Entity2D represents objects in the scene
  class Entity2D final : public Engine::Reflection::Reflectable {
    SERIALIZE_ENTITY2D
      friend class Game2D;
      friend class Transform2D;
      friend class Physics::Physics2D;
      friend class Rendering::Renderable2D;
    public:
      /// The name of the entity.
      std::string name;

      /// Equality operator that checks if the current entity is the same as the given entity
      bool operator==(const Entity2D &entity) const;
      /// Inequality operator that checks if the current entity is the same as the given entity
      bool operator!=(const Entity2D &entity) const;

      /// @returns The pointer to this entity's transform component
      Transform2D *Transform() const;

      /// Creates a component of the given type and adds it to the entity
      template<typename T, typename... Args> requires
        std::is_base_of_v<Component2D, T> && (!std::is_same_v<T, Transform2D>)
      T *AddComponent(Args &&... args) {
        ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSubSystem);

        auto component = std::unique_ptr<T>(new T(std::forward<Args>(args)...));
        component->setEntity(this);
        T *ptr = component.get();
        components.emplace_back(std::move(component));
        if constexpr (std::is_base_of_v<Behaviour, T>) {
          auto behaviour = static_cast<Behaviour *>(ptr);
          behaviour->OnInitialize();
          behaviours.emplace_back(behaviour);
        } else
          ptr->forward();
        return ptr;
      }

      /**
       * Try's to find a component of the given type on the current entity
       * @tparam T The type of the component to find, must inherit from Component2D
       * @return The first component that matches the given type found on the current entity, nullptr if none were found
       */
      template<typename T> requires std::is_base_of_v<Component2D, T>
      [[nodiscard]] T *GetComponent() const {
        ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

        if constexpr (std::is_same_v<T, Transform2D>)
          return transform.get();
        for (auto &component: components)
          if (auto casted = dynamic_cast<T *>(component.get()))
            return casted;
        return nullptr;
      }

      /**
       * Try's to find a component of the given type on the current entity
       * @tparam T The type of the component to find, must inherit from Component2D
       * @return The first component that matches the given type found on the current entity, nullptr if none were found
       */
      template<typename T> requires std::is_base_of_v<Component2D, T>
      [[nodiscard]] bool HasComponent() const {
        ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

        return GetComponent<T>() != nullptr;
      }

      /// Removes the given component to the current entity
      template<typename T> requires std::is_base_of_v<Component2D, T> && (!std::is_same_v<T, Transform2D>)
      void RemoveComponent(const T *component) {
        ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSubSystem);

        if constexpr (std::is_base_of_v<Behaviour, T>) {
          const auto behaviour = static_cast<Behaviour *>(component);
          behaviour->OnDestroy();
          std::erase_if(
            behaviours, [component](const std::unique_ptr<Component2D> &comp) {
              return comp.get() == component;
            }
          );
        } else
          component->recall();

        std::erase_if(
          components, [component](const std::unique_ptr<Component2D> &comp) {
            return comp.get() == component;
          }
        );
      }

      /**
       * Try's to find multiple components of the given type on the current entity
       * @tparam T The type of the component to find, must inherit from Component2D
       * @return The components that match the given type found on the current entity, nullptr if none were found
       */
      template<typename T> requires std::is_base_of_v<Component2D, T>
      [[nodiscard]] std::vector<T *> GetComponents() const {
        ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

        if constexpr (std::is_same_v<T, Transform2D>)
          return {transform.get()};
        std::vector<std::shared_ptr<T>> res;
        for (auto &component: components)
          if (auto casted = dynamic_cast<T *>(component.get()))
            res.push_back(casted);
        return res;
      }

      /// Changes the active state of this entity
      void SetActive(bool active);
      /// @returns True if this entity and all it's parents are enabled, false if not
      [[nodiscard]] bool IsActive() const;
      /// @returns True if this entity is static, false if not
      [[nodiscard]] bool IsStatic() const;
      /// Removes this Entity2D and all it's attached components from the game
      void Destroy();
    protected:
      Entity2D();

      /**
       * Constructs an Entity2D with a given name.
       * @param name The name of the entity.
       * @param isStatic Whether this entity is static in the scene
       * @param position The position of the entity
       * @param rotation The rotation of the entity
       * @param scale The scale of the entity
       * @param parent The parent of this entity
       */
      explicit Entity2D(
        std::string name, bool isStatic = false, glm::vec2 position = glm::vec2(0.0f, 0.0f),
        float rotation = 0.0f, glm::vec2 scale = glm::vec2(1.0f, 1.0f),
        Entity2D *parent = nullptr
      );
    private:
      /// If the current entity is active in the scene
      bool active;
      /// If the parents of the current entity are active in the scene
      bool parentsActive;
      /// Whether this entity is static
      bool isStatic;
      /// Whether this entity is now destroyed and no longer active
      bool destroyed;
      /// How many frames ago was this entity destroyed
      int framesSinceDestroyed;

      /// The transform representing the position, rotation, and scale of the entity in the game world.
      std::unique_ptr<Transform2D> transform;
      /// The list of built-in components linked to this entity
      std::vector<std::unique_ptr<Component2D>> components;
      /// The list of user defined components linked to this entity
      std::vector<Behaviour *> behaviours;

      /// Initializes the entity, setting its parent to the main parent if none is set.
      void initialize() const;
      /// Cleans up resources when the game ends
      void destroy();
      /// Free's up all the memory used by this entity
      void free();
  };
}

#endif // ENTITY2D_H
