//
// Entity2D.h
// Author: Antoine Bastide
// Date: 03/11/2024
//

#ifndef ENTITY2D_H
#define ENTITY2D_H

#include <string>
#include <unordered_set>
#include <vector>
#include <glm/glm.hpp>

#include "2D/Component2D.h"
#include "2D/Transform2D.h"
#include "2D/Physics/Collider2D.h"
#include "2D/Rendering/Texture2D.h"

namespace Engine2D::Physics {
  class Rigidbody2D;
}
using Engine2D::Rendering::Texture2D;

namespace Engine2D {
  /**
   * Represents a 2D entity with a transform, rendering capabilities, and hierarchical structure.
   *
   * The Entity2D class provides an interface for creating entities in a 2D game world.
   * Each entity can have a position, rotation, and scale, a parent entity and multiple components to interact with the 2D game world.
   */
  class Entity2D : public std::enable_shared_from_this<Entity2D> {
    friend class Game2D;
    friend class Transform2D;
    public:
      /// The name of the entity.
      std::string name;
      /// The transform representing the position, rotation, and scale of the entity in the game world.
      Transform2D transform;
      /// The color of the texture
      glm::vec3 textureColor;

      /**
       * Constructs an Entity2D with a given name.
       * @param name The name of the entity.
       */
      explicit Entity2D(std::string name);
      virtual ~Entity2D() = default;

      /// Equality operator that checks if the current entity is the same as the given entity
      bool operator==(const Entity2D &entity) const;
      /// Inequality operator that checks if the current entity is the same as the given entity
      bool operator!=(const Entity2D &entity) const;

      /// Called before the first update, allowing derived classes to customize behavior.
      virtual void OnInitialize() {}
      /// Called once per frame, allowing derived classes to customize behavior.
      virtual void OnUpdate() {}
      /// Called once per physics update, allowing derived classes to customize behavior.
      virtual void OnFixedUpdate() {}
      /// Called when the entity is removed from the game or when the game quits, allowing derived classes to customize behavior.
      virtual void OnDestroy() {}

      /// Called when this entity first collides with a given entity, required the entity to have a Collider2D component
      virtual void OnCollisionEnter2D(const std::shared_ptr<Physics::Collider2D> &collider) {}
      /// Called while this entity collides with a given entity, required the entity to have a Collider2D component
      virtual void OnCollisionStay2D(const std::shared_ptr<Physics::Collider2D> &collider) {}
      /// Called when this entity stops collides with a given entity, required the entity to have a Collider2D component
      virtual void OnCollisionExit2D(const std::shared_ptr<Physics::Collider2D> &collider) {}

      /// Called when this entity is first triggered by a given entity, required the entity to have a Collider2D component
      virtual void OnTriggerEnter2D(const std::shared_ptr<Physics::Collider2D> &collider) {}
      /// Called when this entity is first triggered by a given entity, required the entity to have a Collider2D component
      virtual void OnTriggerStay2D(const std::shared_ptr<Physics::Collider2D> &collider) {}
      /// Called when this entity is stops triggered by a given entity, required the entity to have a Collider2D component
      virtual void OnTriggerExit2D(const std::shared_ptr<Physics::Collider2D> &collider) {}

      /// Creates a component of the given type and adds it to the entity
      template<typename T> requires std::is_base_of_v<Component2D, T> && (!std::is_same_v<T, Transform2D>)
      std::shared_ptr<T> AddComponent() {
        auto component = std::make_shared<T>();
        component->setEntity(shared_from_this());
        components.insert(component);
        forwardComponent(component);
        return component;
      }

      /// Removes the given component to the current entity
      template<typename T> requires std::is_base_of_v<Component2D, T> && (!std::is_same_v<T, Transform2D>)
      void RemoveComponent(const std::shared_ptr<T> &component) {
        recallComponent(component);
        components.erase(component);
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
      [[nodiscard]] std::unordered_set<std::shared_ptr<T> > GetComponents() const {
        if constexpr (std::is_same_v<T, Transform2D>)
          return {transform};
        std::unordered_set<std::shared_ptr<T> > res;
        for (auto component: components)
          if (auto casted = std::dynamic_pointer_cast<T>(component))
            res.insert(casted);
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
      /// Whether this entity has been initialized by the game
      bool initialized;
      /// If the current entity is active in the scene
      bool active;
      /// If the parents of the current entity are active in the scene
      bool parentsActive;
      /// The texture of this entity
      std::shared_ptr<Texture2D> texture;
      /// The components linked to this entity
      std::unordered_set<std::shared_ptr<Component2D> > components;

      /// Initializes the entity, setting its parent to the main parent if none is set.
      void initialize();
      /// Called when the game is updating, allowing derived classes to customize behavior.
      void update();
      /// Cleans up resources when the game ends
      void destroy();

      /// Checks if the given component needs to be sent to other classes
      static void forwardComponent(const std::shared_ptr<Component2D> &component);
      /// Checks if the given component needs to be removed from other classes
      static void recallComponent(const std::shared_ptr<Component2D> &component);
  };
}

#endif // ENTITY2D_H
