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

namespace Engine2D {
  class Game2D;

  // TODO: Add an ID system with generation bits: int id = generation{8}identifier{24}
  // TODO: When ID is implemented, allow smaller or bigger id's for games with less entities i.e. id is int8, int16, int32 or int64
  /**
   * Represents a 2D entity with a transform, rendering capabilities, and hierarchical structure.
   *
   * The Entity2D class provides an interface for creating and managing entities in a 2D game world.
   * Each entity can have a position, rotation, and scale, as well as a renderer and a parent entity.
   */
  class Entity2D {
    public:
      /** The name of the entity. */
      std::string name;
      /** The parent entity of this entity, used for hierarchical transformations. */
      Entity2D *parent;
      /** The transform representing the position, rotation, and scale of the entity in the game world. */
      Transform2D transform;
      /** The texture of this entity */
      Rendering::Texture2D *texture;

      /**
       * Constructs an Entity2D with a given name.
       * @param name The name of the entity.
       * @param parent The parent of this entity (optional)
       * @param texture The texture of this entity (optional)
       */
      explicit Entity2D(std::string name, Entity2D *parent = nullptr, Rendering::Texture2D *texture = nullptr);
      /** Equality operator that checks if the current entity is the same as the given entity */
      bool operator==(const Entity2D &entity) const;

      /** Destructor for the Entity2D class. */
      virtual ~Entity2D() = default;

      /** Called during initialization, allowing derived classes to customize behavior. */
      virtual void Initialize() {}

      /** Called when the game is processing the user input's, allowing derived classes to customize behavior. */
      virtual void ProcessInput() {}

      /** Called when the game is updating, allowing derived classes to customize behavior. */
      virtual void Update() {}

      /** Called when the entity is removed from the game or when the game quits, allowing derived classes to customize behavior.*/
      virtual void Quit() {}

      /**
       * Sets the parent entity of this entity for hierarchical organization.
       * @param parent A pointer to the new parent Entity2D, or nullptr to remove the parent.
       */
      void SetParent(Entity2D *parent);

      friend class Engine2D::Game2D;
    private:
      /** Whether this entity has been initialized by the game */
      bool initialized;

      /** Initializes the entity, setting its parent to the main parent if none is set. */
      void initialize();

      /** Cleans up resources when the game ends */
      void quit();
  };
} // namespace Engine2D

#endif // ENTITY2D_H
