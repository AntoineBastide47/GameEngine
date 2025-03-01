//
// Entity2D.h
// Author: Antoine Bastide
// Date: 03/11/2024
//

#ifndef ENTITY2D_H
#define ENTITY2D_H

#include <string>
#include <entt.hpp>

#include "Engine2D/Transform2D.h"
#include "Engine2D/Physics/Collider2D.h"
#include "Engine2D/Rendering/Texture2D.h"

using Engine2D::Rendering::Texture2D;

namespace Engine2D {
  class Component2D;
  class Behaviour;

  namespace Physics {
    class Physics2D;
  }

  /// Entity2D represents objects in the scene
  class Entity2D final : public std::enable_shared_from_this<Entity2D> {
    friend class Game2D;
    friend class Transform2D;
    friend class Physics::Physics2D;
    public:
      /// The name of the entity.
      std::string name;
      /// The transform representing the position, rotation, and scale of the entity in the game world.
      std::shared_ptr<Transform2D> transform;
      /// The color of the texture
      glm::vec4 textureColor;

      /**
       * Constructs an Entity2D with a given name.
       * @param name The name of the entity.
       */
      explicit Entity2D(std::string name);

      /// Equality operator that checks if the current entity is the same as the given entity
      bool operator==(const Entity2D &entity) const;
      /// Inequality operator that checks if the current entity is the same as the given entity
      bool operator!=(const Entity2D &entity) const;

      /// Creates a component of the given type and adds it to the entity
      template<typename T> requires std::is_base_of_v<Component2D, T> && (!std::is_same_v<T, Transform2D>)
      std::shared_ptr<T> AddComponent() {
        auto component = std::make_shared<T>();
        component->setEntity(shared_from_this());
        if (auto behaviour = std::dynamic_pointer_cast<Behaviour>(component)) {
          behaviours.emplace_back(behaviour);
          behaviour->OnInitialize();
        } else {
          components.emplace_back(component);
          forwardComponent(component);
        }
        return component;
      }

      /// Removes the given component to the current entity
      template<typename T> requires std::is_base_of_v<Component2D, T> && (!std::is_same_v<T, Transform2D>)
      void RemoveComponent(const std::shared_ptr<T> &component) {
        if (auto behaviour = std::dynamic_pointer_cast<Behaviour>(component)) {
          behaviour->OnDestroy();
          std::erase(behaviours, behaviour);
        } else {
          recallComponent(component);
          std::erase(components, component);
        }
      }

      /**
       * Try's to find a component of the given type on the current entity
       * @tparam T The type of the component to find, must inherit from Component2D
       * @return The first component that matches the given type found on the current entity, nullptr if none were found
       */
      template<typename T> requires std::is_base_of_v<Component2D, T>
      [[nodiscard]] bool HasComponent() const {
        if constexpr (std::is_same_v<T, Transform2D>)
          return true;
        else if constexpr (std::is_base_of_v<Behaviour, T>)
          for (const auto &component: behaviours)
            if (auto casted = std::dynamic_pointer_cast<T>(component))
              return true;
        for (auto component: components)
          if (auto casted = std::dynamic_pointer_cast<T>(component))
            return true;
        return false;
      }

      /**
       * Try's to find a component of the given type on the current entity
       * @tparam T The type of the component to find, must inherit from Component2D
       * @return The first component that matches the given type found on the current entity, nullptr if none were found
       */
      template<typename T> requires std::is_base_of_v<Component2D, T>
      [[nodiscard]] std::shared_ptr<T> GetComponent() const {
        if constexpr (std::is_same_v<T, Transform2D>)
          return transform;
        else if constexpr (std::is_base_of_v<Behaviour, T>)
          for (auto component: behaviours)
            if (auto casted = std::dynamic_pointer_cast<T>(component))
              return casted;
        for (auto component: components)
          if (auto casted = std::dynamic_pointer_cast<T>(component))
            return casted;
        return nullptr;
      }

      /**
       * Try's to find multiple components of the given type on the current entity
       * @tparam T The type of the component to find, must inherit from Component2D
       * @return The components that match the given type found on the current entity, nullptr if none were found
       */
      template<typename T> requires std::is_base_of_v<Component2D, T>
      [[nodiscard]] std::vector<std::shared_ptr<T>> GetComponents() const {
        if constexpr (std::is_same_v<T, Transform2D>)
          return {transform};
        std::vector<std::shared_ptr<T>> res;
        if constexpr (std::is_base_of_v<Behaviour, T>) {
          for (auto component: behaviours)
            if (auto casted = std::dynamic_pointer_cast<T>(component))
              res.push_back(casted);
          return res;
        }
        for (auto component: components)
          if (auto casted = std::dynamic_pointer_cast<T>(component))
            res.push_back(casted);
        return res;
      }

      /// Changes the active state of this entity
      void SetActive(bool active);
      /// @returns True if this entity and all it's parents are enabled, false if not
      [[nodiscard]] bool IsActive() const;
      /// Sets the texture of the entity to the given texture
      void SetTexture(const std::shared_ptr<Texture2D> &texture);
      /// @returns The pointer to the texture of this entity
      [[nodiscard]] std::shared_ptr<Texture2D> Texture() const;
      /// Removes this Entity2D and all it's attached components from the game
      void Destroy();
    private:
      /// If the current entity is active in the scene
      bool active;
      /// If the parents of the current entity are active in the scene
      bool parentsActive;
      /// The texture of this entity
      std::shared_ptr<Texture2D> texture;
      /// The list of built-in components linked to this entity
      std::vector<std::shared_ptr<Component2D>> components;
      /// The list of user defined components linked to this entity
      std::vector<std::shared_ptr<Behaviour>> behaviours;

      /// Initializes the entity, setting its parent to the main parent if none is set.
      void initialize();
      /// Cleans up resources when the game ends
      void destroy();

      /// Checks if the given component needs to be sent to other classes
      static void forwardComponent(const std::shared_ptr<Component2D> &component);
      /// Checks if the given component needs to be removed from other classes
      static void recallComponent(const std::shared_ptr<Component2D> &component);
  };
}

#endif // ENTITY2D_H
