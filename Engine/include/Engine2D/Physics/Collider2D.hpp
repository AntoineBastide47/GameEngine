//
// Collider2D.h
// Author: Antoine Bastide
// Date: 22/12/2024
//

#ifndef COLLIDER2D_H
#define COLLIDER2D_H

#include <vector>
#include <glm/glm.hpp>

#include "Engine2D/Component2D.hpp"
#include "Engine/Types/float01.hpp"
#include "Collider2D.gen.hpp"

namespace Engine2D {
  class Entity2D;
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
    SERIALIZE_COLLIDER2D
      friend class Physics2D;
      friend class Collisions;
      friend class Rigidbody2D;
      friend class CollisionGrid;
      friend class Engine2D::Entity2D;
    public:
      enum Type {
        None = -1, Circle = 0, Rectangle = 1, Polygon = 2,
      };

      /// Coefficient of bounciness  of the rigid body.
      Engine::float01 elasticity;
      /// The positional offset of the collider in regard to the position of the entity it is attached to
      glm::vec2 offset;
      /// If true, the colliders bounds is computed using the transform of the entity it is attached to (works in most cases).
      /// If false, you need to specify the bounds of the entity manually (ex: width, height, radius).
      bool autoCompute;
      /// The position of the collider if it is not auto computed
      glm::vec2 position;
      /// Determines whether the collider is configured as a trigger.
      bool isTrigger;

      /// @returns The points at which this rigidbody collided with another rigidbody
      [[nodiscard]] std::vector<glm::vec2> ContactPoints() const;
      Type GetType() const;
    protected:
      struct AABB final : Reflectable {
        SERIALIZE_AABB
          glm::vec2 min;
          glm::vec2 max;

          AABB()
            : min(glm::vec2(0)), max(glm::vec2(0)) {}
      };

      /// The type of this collider
      ENGINE_SERIALIZE Type type;
      /// The vertices that make up the initial collider.
      ENGINE_SERIALIZE std::vector<glm::vec2> vertices;
      /// The vertices that make up the initial collider transformed to match its current position, rotation and scale.
      std::vector<glm::vec2> transformedVertices;
      /// Weather or not this collider has been initialized or not
      bool initialized;
      /// The axis-aligned bounding box  of this rigidbody.
      AABB aabb;
      /// The model matrix of the rigidbody last time it changed
      glm::mat4 lastModelMatrix;
      /// The rigidbody attached to the entity this collider is attached to
      Rigidbody2D *rigidbody;
      /// The points at which this rigidbody collided with another rigidbody
      std::vector<glm::vec2> contactPoints;

      Collider2D();

      void forward() override;
      void recall() override;

      /// Calculates the AABB of for the given collider type
      virtual void computeAABB() {}

      /// @returns The current position of the collider taking into account if it is auto computed or not
      [[nodiscard]] glm::vec2 getPosition() const;

      /// @returns The current scale of the collider taking into account if it is auto computed or not
      [[nodiscard]] virtual glm::vec2 getScale() const {
        return {};
      }
    private:
      /// @return A std::array containing the min and max points of the AABB.
      [[nodiscard]] AABB getAABB();
  };

  /** Collider for 2D physics representing a circle. */
  class CircleCollider2D final : public Collider2D {
    SERIALIZE_CIRCLECOLLIDER2D
      friend class Engine2D::Entity2D;
    public:
      /// The radius of this collider
      float radius;
    protected:
      CircleCollider2D();
      void computeAABB() override;
      glm::vec2 getScale() const override;
  };

  /** Collider for 2D physics representing a rectangle. */
  class BoxCollider2D final : public Collider2D {
    SERIALIZE_BOXCOLLIDER2D
      friend class Engine2D::Entity2D;
    public:
      /// The height of this collider
      float width;
      /// The width of this collider
      float height;
    protected:
      BoxCollider2D();
      void computeAABB() override;
      glm::vec2 getScale() const override;
  };

  /** Collider for 2D physics representing a polygon. */
  class PolygonCollider2D final : public Collider2D {
    SERIALIZE_POLYGONCOLLIDER2D
      friend class Engine2D::Entity2D;
    public:
      using Collider2D::vertices;
    protected:
      PolygonCollider2D();
      void computeAABB() override;
  };
}

#endif //COLLIDER2D_H
