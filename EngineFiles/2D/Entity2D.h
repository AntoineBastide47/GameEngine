//
// Entity2D.h
// Entity2D: Represents a 2D entity with transform, rendering, and hierarchical capabilities
// Author: Antoine Bastide
// Date: 03/11/2024
//

#ifndef ENTITY2D_H
#define ENTITY2D_H

#include <string>
#include <iostream>
#include <vector>

#include "2D/Components/Component2D.h"
#include "2D/Components/Transform2D.h"
#include "2D/Rendering/Texture2D.h"

namespace Engine2D::Physics {
  class Rigidbody2D;
}
using Engine2D::Rendering::Texture2D;

namespace Engine2D {
  // TODO: Add an ID system
  /**
   * Represents a 2D entity with a transform, rendering capabilities, and hierarchical structure.
   *
   * The Entity2D class provides an interface for creating and managing entities in a 2D game world.
   * Each entity can have a position, rotation, and scale, as well as a renderer and a parent entity.
   */
  class Entity2D {
    friend class Game2D;
    public:
      /** The name of the entity. */
      std::string name;
      /** The transform representing the position, rotation, and scale of the entity in the game world. */
      Transform2D transform;
      /** The texture of this entity */
      Texture2D *texture;
      /** The color of the texture */
      glm::vec3 textureColor;

      /**
       * Constructs an Entity2D with a given name.
       * @param name The name of the entity.
       * @param parent The parent of this entity (optional)
       * @param texture The texture of this entity (optional)
       */
      explicit Entity2D(std::string name, Entity2D *parent = nullptr, Texture2D *texture = nullptr);
      /** Equality operator that checks if the current entity is the same as the given entity */
      bool operator==(const Entity2D &entity) const;

      /** Destructor for the Entity2D class. */
      virtual ~Entity2D() = default;

      /** Called before the first update, allowing derived classes to customize behavior. */
      virtual void Initialize() {}
      /** Called once per frame, allowing derived classes to customize behavior. */
      virtual void Update() {}
      /** Called when the entity is removed from the game or when the game quits, allowing derived classes to customize behavior.*/
      virtual void Quit() {}

      virtual void OnCollision(Physics::Rigidbody2D *collider) {}

      /**
       * Makes this entity be updatable and renderable, or not
       * @param newState If true, this entity will be updated and rendered<br>
       *                 If false, this entity won't be updated and rendered
       */
      void SetActive(bool newState);
      /** @returns True if the current entity is active, False otherwise */
      [[nodiscard]] bool IsActive() const;

      /** Adds the given component to the current entity */
      void AddComponent(Component2D &component);
      /** Removes the given component to the current entity */
      void RemoveComponent(Component2D &component);

      /**
       * Try's to find a component of the given type on the current entity
       * @tparam C The type of the component to find, must inherit from Component2D
       * @return The first component that matches the given type found on the current entity, nullptr if none were found
       */
      template<typename C> requires std::is_base_of_v<Component2D, C>
      [[nodiscard]] C *GetComponent() const {
        if constexpr (std::is_same_v<C, Transform2D>)
          return transform;
        for (auto component: components) {
          if (auto casted = dynamic_cast<C*>(component))
            return casted;
        }
        return nullptr;
      }

      /**
       * Try's to find multiple components of the given type on the current entity
       * @tparam C The type of the component to find, must inherit from Component2D
       * @return The components that match the given type found on the current entity, nullptr if none were found
       */
      template<typename C> requires std::is_base_of_v<Component2D, C>
      [[nodiscard]] std::vector<C *> GetComponents() const {
        if constexpr (std::is_same_v<C, Transform2D>)
          return {transform};
        std::vector<C *> res;
        for (auto component: components)
          if (typeid(component) == typeid(C))
            res.push_back(dynamic_cast<C*>(component));
        return res;
      }

      /** Removes this Entity2D and all it's attached components from the game */
      void Destroy();
    private:
      /** If the current entity is active in the scene, if it is not, it will not be updated and rendered */
      bool active = true;
      /** Whether this entity has been initialized by the game */
      bool initialized = false;
      /** The components linked to this entity */
      std::vector<Component2D *> components;

      /** Initializes the entity, setting its parent to the main parent if none is set. */
      void initialize();
      /** Called when the game is updating, allowing derived classes to customize behavior. */
      void update();
      /** Cleans up resources when the game ends */
      void quit();
  };
} // namespace Engine2D

#endif // ENTITY2D_H
