//
// Collider2D.h
// Author: Antoine Bastide
// Date: 22/12/2024
//

#ifndef COLLIDER2D_H
#define COLLIDER2D_H

#include "2D/Component2D.h"
#include "2D/Types/Vector2.h"
#include "Types/float01.h"

namespace Engine2D {
  class Game2D;
}

namespace Engine2D::Physics {
  class Rigidbody2D;
  class Collider2D : public Component2D {
    friend class Physics2D;
    friend class CollisionGrid;
    friend class Collisions;
    friend class Rigidbody2D;
    friend class Engine2D::Game2D;
    public:
      enum Type {
        None = -1, Circle = 0, Rectangle = 1, Shape = 2,
      };
      /// Coefficient of bounciness  of the rigid body.
      Engine::float01 bounciness;
      /// Determines weather the collider is configured as a trigger.
      bool isTrigger;

      Collider2D();

      /// @returns The points at which this rigidbody collided with another rigidbody
      [[nodiscard]] std::vector<Vector2f> ContactPoints() const;
      Type GetType() const;
    protected:
      struct AABB {
        Vector2f min;
        Vector2f max;
      };
      /// The type of this collider
      Type type;
      /// The vertices that make up the initial collider.
      std::vector<Vector2f> vertices;
      /// The vertices that make up the initial collider transformed to match its current position, rotation and scale.
      std::vector<Vector2f> transformedVertices;
      /// Weather or not this collider has been initialized or not
      bool initialized;
      /// The axis-aligned bounding box  of this rigidbody.
      AABB aabb;
      /// The model matrix of the rigidbody last time it changed
      glm::mat4 lastModelMatrix;
      /// The rigidbody attached to the entity this collider is attached to
      std::shared_ptr<Rigidbody2D> rigidbody;

      virtual void computeAABB() {}
    private:
      /// The points at which this rigidbody collided with another rigidbody
      std::vector<Vector2f> contactPoints;
      /// @return A std::array containing the min and max points of the AABB.
      [[nodiscard]] AABB getAABB();
  };

  class CircleCollider2D : public Collider2D {
    public:
      CircleCollider2D();
    protected:
      void computeAABB() override;
  };

  class RectangleCollider2D : public Collider2D {
    public:
      RectangleCollider2D();
    protected:
      void computeAABB() override;
  };

  class ShapeCollider2D : public Collider2D {
    public:
      using Collider2D::vertices;
      ShapeCollider2D();
    protected:
      void computeAABB() override;
  };
}

#endif //COLLIDER2D_H
