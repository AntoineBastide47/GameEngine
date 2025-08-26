//
// Transform2D.h
// Author: Antoine Bastide
// Date: 03/11/2024
//

#ifndef TRANSFORM2D_H
#define TRANSFORM2D_H

#include <vector>
#include <glm/glm.hpp>

#include "Engine2D/Component2D.hpp"
#include "Transform2D.gen.hpp"

namespace Editor {
  class EntityInspector;
}

namespace Engine2D {
  /**
   * Represents the position, rotation, and scale of a 2D entity.
   * This class is used to define and manage the spatial transformations (position, rotation, scale)
   * of entities in a 2D space. It provides operators for equality comparisons between transforms.
   */
  class Transform2D final : public Component2D {
    SERIALIZE_TRANSFORM2D
      friend class Scene;
      friend class Entity2D;
      friend class Editor::EntityInspector;
    public:
      /// Equality operator to compare two Transform2D objects.
      bool operator==(const Transform2D &other) const;
      /// Inequality operator to compare two Transform2D objects.
      bool operator!=(const Transform2D &transform) const;

      std::vector<Entity2D *>::iterator begin();
      std::vector<Entity2D *>::iterator end();
      [[nodiscard]] std::vector<Entity2D *>::const_iterator begin() const;
      [[nodiscard]] std::vector<Entity2D *>::const_iterator end() const;

      /// @returns The local position of this transform
      [[nodiscard]] glm::vec2 Position() const;
      /// @returns The local scale of this transform
      [[nodiscard]] glm::vec2 Scale() const;
      /// @returns The local rotation of this transform
      [[nodiscard]] float Rotation() const;

      /// Overrides the current local position of this transform with the given position
      /// @param newPosition The new local position to set
      void SetPosition(glm::vec2 newPosition);

      /// Overrides the current local rotation of this transform with the given rotation
      /// @param newRotation The new local rotation to set
      void SetRotation(float newRotation);

      /// Overrides the current local scale of this transform with the given scale
      /// @param newScale The new local scale to set
      void SetScale(glm::vec2 newScale);

      /// Overrides the current local position and rotation of this transform with the given values
      /// @param newPosition The new local position to set
      /// @param newRotation The new local rotation to set
      void SetPositionAndRotation(glm::vec2 newPosition, float newRotation);

      /// Overrides the current local position and scale of this transform with the given values
      /// @param newPosition The new local position to set
      /// @param newScale The new local scale to set
      void SetPositionAndScale(glm::vec2 newPosition, glm::vec2 newScale);

      /// Overrides the current local rotation and scale of this transform with the given values
      /// @param newRotation The new local rotation to set
      /// @param newScale The new local scale to set
      void SetRotationAndScale(float newRotation, glm::vec2 newScale);

      /// Overrides the current local position, rotation, and scale of this transform with the given values
      /// @param newPosition The new local position to set
      /// @param newRotation The new local rotation to set
      /// @param newScale The new local scale to set
      void SetPositionRotationAndScale(glm::vec2 newPosition, float newRotation, glm::vec2 newScale);

      /// Adds the given position increment to the current local position of this transform
      /// @param positionIncrement The increment to add to the current local position
      void UpdatePosition(glm::vec2 positionIncrement);

      /// Adds the given rotation increment to the current local rotation of this transform
      /// @param rotationIncrement The increment to add to the current local rotation
      void UpdateRotation(float rotationIncrement);

      /// Adds the given scale increment to the current local scale of this transform
      /// @param scaleIncrement The increment to add to the current local scale
      void UpdateScale(glm::vec2 scaleIncrement);

      /// Adds the given position and rotation increments to the current local position and rotation of this transform
      /// @param positionIncrement The increment to add to the current local position
      /// @param rotationIncrement The increment to add to the current local rotation
      void UpdatePositionAndRotation(glm::vec2 positionIncrement, float rotationIncrement);

      /// Adds the given position and scale increments to the current local position and scale of this transform
      /// @param positionIncrement The increment to add to the current local position
      /// @param scaleIncrement The increment to add to the current local scale
      void UpdatePositionAndScale(glm::vec2 positionIncrement, glm::vec2 scaleIncrement);

      /// Adds the given rotation and scale increments to the current local rotation and scale of this transform
      /// @param rotationIncrement The increment to add to the current local rotation
      /// @param scaleIncrement The increment to add to the current local scale
      void UpdateRotationAndScale(float rotationIncrement, glm::vec2 scaleIncrement);

      /// Adds the given position, rotation, and scale increments to the current local position, rotation, and scale of this transform
      /// @param positionIncrement The increment to add to the current local position
      /// @param rotationIncrement The increment to add to the current local rotation
      /// @param scaleIncrement The increment to add to the current local scale
      void UpdatePositionRotationAndScale(
        glm::vec2 positionIncrement, float rotationIncrement, glm::vec2 scaleIncrement
      );

      /// @returns A unit vector representing the forward direction of this entity.
      [[nodiscard]] glm::vec2 Forward() const;
      /// @returns A unit vector representing the up direction of this entity.
      [[nodiscard]] glm::vec2 Up() const;

      /// Sets the parent entity of this entity for hierarchical organization.
      /// @param parent A pointer to the new parent Entity2D, or nullptr to remove the parent.
      /// @note Does nothing if the entity this transform is attached to is static
      void SetParent(Entity2D *parent);
      /// @returns The parent of the Entity2D this transform is attached to
      [[nodiscard]] Entity2D *Parent() const;
      /// @returns True if the given entity is a parent of the Entity2D this transform is attached to
      [[nodiscard]] bool IsChildOf(const Entity2D *entity) const;
      /// @returns True if the given entity is a child of the Entity2D this transform is attached to
      [[nodiscard]] bool IsParentOf(const Entity2D *entity) const;

      /// @returns False if the transform of this entity is not entirely in the viewport of the screen, True if not
      [[nodiscard]] bool IsVisible() const;
      /// @returns The position of the Entity2D this transform is attached to in world coordinates
      [[nodiscard]] glm::vec2 WorldPosition() const;
      /// @returns The rotation of the Entity2D this transform is attached to in world coordinates
      [[nodiscard]] float WorldRotation() const;
      /// @returns The scale of the Entity2D this transform is attached to in world coordinates
      [[nodiscard]] glm::vec2 WorldScale() const;
      /// @returns The scale of the Entity2D this transform is attached to in world coordinates divided by 2
      [[nodiscard]] glm::vec2 WorldHalfScale() const;
      /// @returns The transform matrix of this entity
      [[nodiscard]] const glm::mat4 &WorldMatrix();

      /// @returns The given point for world to local coordinates
      [[nodiscard]] glm::vec2 WorldToLocal(const glm::vec2 &point);

      [[nodiscard]] const std::vector<Entity2D *> &Children() const;
      /// Sets the parent of all the children of the Entity2D this transform is attached to the current scene's root
      void RemoveAllChildren();
      /// @returns The child with the given name if it was found, nullptr if not
      [[nodiscard]] Entity2D *Find(const std::string &name) const;

      /// @returns The position of this child, -1 if it isn't a child of this entity
      [[nodiscard]] int ChildIndex(const Entity2D *child) const;
      /// @returns The child at the given index
      /// @note Log's an error if index is out of bounds
      [[nodiscard]] Entity2D *ChildAt(size_t index) const;
      /// Puts the given child at the start of the list of children attached to the Entity2D this component is attached to
      void MakeFirstChild(Entity2D *child);
      /// Puts the given child at the nth position of the list of children attached to the Entity2D this component is attached to
      void MakeNthChild(Entity2D *child, size_t n);
      /// Puts the given child at the end of list of children attached to the Entity2D this component is attached to
      void MakeLastChild(Entity2D *child);
      /// @returns The number of children this transform has
      [[nodiscard]] size_t ChildCount() const;

      void OnSerialize(Engine::Reflection::Format format, Engine::JSON &json) const override;
    private:
      using Component2D::Transform; // Disallow unnecessary overhead to access this component

      /// Position of the transform in 2D space.
      ENGINE_SERIALIZE glm::vec2 position;
      /// Position of the transform in 2D space.
      ENGINE_SERIALIZE_HIDDEN glm::vec2 worldPosition;
      /// Rotation of the transform in degrees.
      ENGINE_SERIALIZE float rotation;
      /// Rotation of the transform in degrees.
      ENGINE_SERIALIZE_HIDDEN float worldRotation;
      /// Scale of the transform in 2D space.
      ENGINE_SERIALIZE glm::vec2 scale;
      /// Scale of the transform in 2D space.
      ENGINE_SERIALIZE_HIDDEN glm::vec2 worldScale;

      /// The parent Entity2D of the entity that this transform is attached to
      Entity2D *parent;
      /// The list of all the children of this entity
      std::vector<Entity2D *> children;

      /// If the transform matrix needs to be recomputed
      bool dirty;
      /// If this entity is on screen
      bool visible;
      /// Transform matrix
      glm::mat4 projectionMatrix;

      Transform2D();

      /**
       * @brief Parameterized constructor that initializes position, rotation, and scale to specified values.
       * @param position Initial position of the transform, defaulted to (0, 0).
       * @param rotation Initial rotation in degrees, defaulted to 0.
       * @param scale Initial scale of the transform, defaulted to (1, 1).
       * @param entity The entity this component is attached to
       * @param parent The parent of the Entity this component is attached to
       */
      Transform2D(glm::vec2 position, float rotation, glm::vec2 scale, Entity2D *entity, Entity2D *parent = nullptr);

      /// Callback function that updates fields of this transform only if any of its public properties are modified
      void onTransformChange();
      /// Callback function that updates the active state of the entity when it's parent list changes
      void onParentHierarchyChange() const;

      /// Adds the given child to the child list of the current entity's transform
      void addChild(Entity2D *child);

      void OnEditorValueChanged() override;
  };
} // namespace Engine2D

#endif // TRANSFORM2D_H
