//
// Entity2D.h
// Entity2D: Represents a 2D entity with transform, rendering, and hierarchical capabilities
// Author: Antoine Bastide
// Date: 03/11/2024
//

#ifndef ENTITY2D_H
#define ENTITY2D_H

#include <string>
#include "2D/Components/Transform2D.h"
#include "Rendering/Texture2D.h"

using Engine2D::Rendering::Texture2D;

namespace Engine2D {
  class Game2D;

  // TODO: Add an ID system
  /**
   * Represents a 2D entity with a transform, rendering capabilities, and hierarchical structure.
   *
   * The Entity2D class provides an interface for creating and managing entities in a 2D game world.
   * Each entity can have a position, rotation, and scale, as well as a renderer and a parent entity.
   */
  class Entity2D {
    friend class Engine2D::Game2D;
    public:
      /** The name of the entity. */
      std::string name;
      /** The parent entity of this entity, used for hierarchical transformations. */
      Entity2D *parent;
      /** The transform representing the position, rotation, and scale of the entity in the game world. */
      Transform2D transform;
      /** The texture of this entity */
      Texture2D *texture;
      /** The color of the texture */
      glm::vec3 *textureColor;

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

      /** Called during initialization, allowing derived classes to customize behavior. */
      virtual void Initialize() {}
      /** Called when the game is updating, allowing derived classes to customize behavior. */
      virtual void Update() {}
      /** Called when the entity is removed from the game or when the game quits, allowing derived classes to customize behavior.*/
      virtual void Quit() {}

      /**
       * Sets the parent entity of this entity for hierarchical organization.
       * @param parent A pointer to the new parent Entity2D, or nullptr to remove the parent.
       */
      void SetParent(Entity2D *parent);
      /**
       * Makes this entity be updatable and renderable, or not
       * @param newState If true, this entity will be updated and rendered<br>
       *                 If false, this entity won't be updated and rendered
       */
      void SetActive(bool newState);
      [[nodiscard]] bool IsActive() const;
    private:
      /** If the current entity is active in the scene, if it is not, it will not be updated and rendered */
      bool active = true;
      /** Whether this entity has been initialized by the game */
      bool initialized = false;

      /** Initializes the entity, setting its parent to the main parent if none is set. */
      void initialize();
      /** Called when the game is updating, allowing derived classes to customize behavior. */
      void update();
      /** Cleans up resources when the game ends */
      void quit();
  };
} // namespace Engine2D

#endif // ENTITY2D_H
