//
// Transform2D.h
// Author: Antoine Bastide
// Date: 03/11/2024
//

#ifndef TRANSFORM2D_H
#define TRANSFORM2D_H

#include <vector>

#include "2D/Component2D.h"
#include "2D/Types/Vector2.h"
#include "Common/Property.h"

namespace Engine2D {
  class Entity2D;
  /**
   * Represents the position, rotation, and scale of a 2D entity.
   * This class is used to define and manage the spatial transformations (position, rotation, scale)
   * of entities in a 2D space. It provides operators for equality comparisons between transforms.
   */
  class Transform2D : public Component2D {
    friend class Game2D;
    friend class Entity2D;
    public:
      /// Position of the transform in 2D space.
      Engine::Property<Vector2f> position;
      /// Rotation of the transform in degrees.
      Engine::Property<float> rotation;
      /// Scale of the transform in 2D space.
      Engine::Property<Vector2f> scale;

      /**
       * @brief Parameterized constructor that initializes position, rotation, and scale to specified values.
       * @param position Initial position of the transform, defaulted to (0, 0).
       * @param rotation Initial rotation in degrees, defaulted to 0.
       * @param scale Initial scale of the transform, defaulted to (1, 1).
       */
      explicit Transform2D(Vector2f position = Vector2f::Zero, float rotation = 0, Vector2f scale = Vector2f::One);
      ~Transform2D() override;

      /// Equality operator to compare two Transform2D objects.
      bool operator==(const Transform2D &other) const;
      /// Inequality operator to compare two Transform2D objects.
      bool operator!=(const Transform2D &transform) const;

      std::vector<std::shared_ptr<Entity2D> >::iterator begin();
      std::vector<std::shared_ptr<Entity2D> >::iterator end();
      [[nodiscard]] std::vector<std::shared_ptr<Entity2D> >::const_iterator begin() const;
      [[nodiscard]] std::vector<std::shared_ptr<Entity2D> >::const_iterator end() const;

      /// @returns A unit vector representing the forward direction of this entity.
      [[nodiscard]] Vector2f Forward() const;
      /// @returns A unit vector representing the up direction of this entity.
      [[nodiscard]] Vector2f Up() const;

      /**
       * Sets the parent entity of this entity for hierarchical organization.
       * @param parent A pointer to the new parent Entity2D, or nullptr to remove the parent.
       */
      void SetParent(std::shared_ptr<Entity2D> parent);
      /// @returns The parent of the Entity2D this transform is attached to
      [[nodiscard]] std::shared_ptr<Entity2D> GetParent() const;
      /// @returns True if the given entity is a parent of the Entity2D this transform is attached to
      [[nodiscard]] bool IsChildOf(const std::shared_ptr<Entity2D> &entity) const;
      /// @returns True if the given entity is a child of the Entity2D this transform is attached to
      [[nodiscard]] bool IsParentOf(const std::shared_ptr<Entity2D> &entity) const;

      /// @returns True if the component's properties were changed last frame
      [[nodiscard]] bool Changed() const;
      /// @returns The position of the Entity2D this transform is attached to in world coordinates
      [[nodiscard]] Vector2f WorldPosition() const;
      /// @returns The rotation of the Entity2D this transform is attached to in world coordinates
      [[nodiscard]] float WorldRotation() const;
      /// @returns The scale of the Entity2D this transform is attached to in world coordinates
      [[nodiscard]] Vector2f WorldScale() const;
      /// @returns The scale of the Entity2D this transform is attached to in world coordinates divided by 2
      [[nodiscard]] Vector2f WorldHalfScale() const;
      /// @returns False if the transform of this entity is not entirely in the viewport of the screen, True if not
      [[nodiscard]] bool Visible() const;
      /// @returns The transform matrix of this entity
      [[nodiscard]] glm::mat4 WorldMatrix() const;

      /// Sets the parent of all the children of the Entity2D this transform is attached to the current scene's root
      void RemoveAllChildren();
      /// @returns The child with the given name if it was found, nullptr if not
      [[nodiscard]] std::shared_ptr<Entity2D> Find(const std::string &name) const;
      /**
       * @returns The child at the given index
       * @note Log's an error if index is out of bounds
       */
      [[nodiscard]] std::shared_ptr<Entity2D> GetChild(int index) const;
      /// Puts the given child at the start of the list of children attached to the Entity2D this component is attached to
      void MakeFirstChild(const std::shared_ptr<Entity2D> &child);
      /// Puts the given child at the end of list of children attached to the Entity2D this component is attached to
      void MakeLastChild(const std::shared_ptr<Entity2D> &child);
    private:
      using Component2D::Transform; // Disallow unnecessary overhead to call this component
      /// The parent Entity2D of the entity that this transform is attached to
      std::shared_ptr<Entity2D> parent;
      /// The list of all the parents of the parent of this entity
      std::vector<std::shared_ptr<Entity2D> > parentList;
      /// The list of all the children of this entity
      std::vector<std::shared_ptr<Entity2D> > childList;

      /// Is the transform was updated last frame
      bool wasUpdated;
      /// Position of the transform in 2D space.
      Vector2f worldPosition;
      /// Rotation of the transform in degrees.
      float worldRotation = 0;
      /// Scale of the transform in 2D space.
      Vector2f worldScale;
      /// Half of the scale of the transform in 2D space.
      Vector2f worldScaleHalf;
      /// If this entity is on screen
      bool visible;
      /// Transform matrix
      glm::mat4 projectionMatrix;

      /// Callback function that updates fields of this transform only if any of its public properties are modified
      void onTransformChange();
      /// Callback function that updates the active state of the entity when it's parent list changes
      void onParentHierarchyChange();

      /// Adds the given child to the child list of the current entity's transform
      void addChild(const std::shared_ptr<Entity2D> &child);
      /// Removes the given child to the child list of the current entity's transform
      void removeChild(const std::shared_ptr<Entity2D> &child);
  };
} // namespace Engine2D

#endif // TRANSFORM2D_H
