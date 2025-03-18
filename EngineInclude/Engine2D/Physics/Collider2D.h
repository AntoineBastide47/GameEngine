//
// Collider2D.h
// Author: Antoine Bastide
// Date: 22/12/2024
//

#ifndef COLLIDER2D_H
#define COLLIDER2D_H

#include <vector>
#include <glm/glm.hpp>

#include "Engine2D/Component2D.h"
#include "Engine/Types/float01.h"

namespace Engine2D {
  class Game2D;
}

namespace Engine2D::Physics {
  class Rigidbody2D;

  /**
   * The Collider2D class serves as a base class for defining different types of colliders used in 2D physics simulations.
   * Colliders are responsible for defining the boundaries and shapes of objects, enabling collision detection and
   * resolution in a 2D physics' engine.
   *
   * This class is intended to be extended to implement specific collider shapes.
   */
  class Collider2D : public Component2D {
    friend class Physics2D;
    friend class CollisionGrid;
    friend class Collisions;
    friend class Rigidbody2D;
    friend class Engine2D::Game2D;
    public:
      enum Type {
        None = -1, Circle = 0, Rectangle = 1, Polygon = 2,
      };
      /// Coefficient of bounciness  of the rigid body.
      Engine::float01 bounciness;
      /// The positional offset of the collider in regard to the position of the entity it is attached to
      glm::vec2 offset;
      /// If true, the colliders bounds is computed using the transform of the entity it is attached to (works in most cases).
      /// If false, you need to specify the bounds of the entity manually (ex: width, height, radius).
      bool autoCompute;
      /// The position of the collider if it is not auto computed
      glm::vec2 position;
      /// Determines whether the collider is configured as a trigger.
      bool isTrigger;
      /// If the bounds of the collider should be drawn to the screen or not
      bool draw;

      /// @returns The points at which this rigidbody collided with another rigidbody
      [[nodiscard]] std::vector<glm::vec2> ContactPoints() const;
      Type GetType() const;
    protected:
      struct AABB {
        glm::vec2 min;
        glm::vec2 max;

        AABB() : min(glm::vec2(0)), max(glm::vec2(0)) {}
      };
      /// The type of this collider
      Type type;
      /// The vertices that make up the initial collider.
      std::vector<glm::vec2> vertices;
      /// The vertices that make up the initial collider transformed to match its current position, rotation and scale.
      std::vector<glm::vec2> transformedVertices;
      /// Weather or not this collider has been initialized or not
      bool initialized;
      /// The axis-aligned bounding box  of this rigidbody.
      AABB aabb;
      /// The model matrix of the rigidbody last time it changed
      glm::mat4 lastModelMatrix;
      /// The rigidbody attached to the entity this collider is attached to
      std::shared_ptr<Rigidbody2D> rigidbody;

      Collider2D();

      /// Calculates the AABB of for the given collider type
      virtual void computeAABB() {}

      /// @returns The current position of the collider taking into account if it is auto computed or not
      [[nodiscard]] glm::vec2 getPosition() const;

      /// @returns The current scale of the collider taking into account if it is auto computed or not
      [[nodiscard]] virtual glm::vec2 getScale() const {
        return {};
      }
    private:
      /// The points at which this rigidbody collided with another rigidbody
      std::vector<glm::vec2> contactPoints;
      /// @return A std::array containing the min and max points of the AABB.
      [[nodiscard]] AABB getAABB();

      void drawGizmos2D() const;
  };

  /** Collider for 2D physics representing a circle. */
  class CircleCollider2D final : public Collider2D {
    public:
      /// The radius of this collider
      float radius;
      CircleCollider2D();
    protected:
      void computeAABB() override;
      glm::vec2 getScale() const override;
  };

  /** Collider for 2D physics representing a rectangle. */
  class BoxCollider2D final : public Collider2D {
    public:
      /// The height of this collider
      float width;
      /// The width of this collider
      float height;
      BoxCollider2D();
    protected:
      void computeAABB() override;
      glm::vec2 getScale() const override;
  };

  /** Collider for 2D physics representing a polygon. */
  class PolygonCollider2D final : public Collider2D {
    public:
      using Collider2D::vertices;
      PolygonCollider2D();
    protected:
      void computeAABB() override;
  };
}

#endif //COLLIDER2D_H
