//
// Rigidbody2D.h
// Author: Antoine Bastide
// Date: 30/11/2024
//

#ifndef RIGIDBODY2D_H
#define RIGIDBODY2D_H

#include "2D/Components/Component2D.h"
#include "2D/Types/Vector2.h"
#include "Common/Property.h"
#include "Types/float01.h"

namespace Engine2D {
  class Game2D;
}

namespace Engine2D::Physics {
  /**
   * @class Rigidbody2D
   * Represents a 2D rigid body in the physics engine.
   *
   * The Rigidbody2D class is responsible for simulating physical behavior such as velocity,
   * forces, collisions, and interaction with gravity. It supports two types of bodies:
   * Circle and Rectangle.
   */
  class Rigidbody2D : public Component2D {
    friend class Engine2D::Game2D;
    friend class Physics2D;
    friend class Collisions;
    friend class CollisionGrid;
    public:
      struct AABB {
        Vector2f min;
        Vector2f max;
      };
      enum Rigidbody2DType {
        Circle, Rectangle
      };

      /// Coefficient of restitution (bounciness) of the rigid body.
      Engine::float01 restitution;
      /// Indicates how much of the angular velocity will be applied during collisions.
      /// For full rotation set it to 1, for no rotation set it to 0 or values inbetween for different reactions.
      Engine::float01 angularDamping;
      /// Flag indicating whether the body is static (immovable).
      bool isStatic;
      /// Radius of the body (if it's a Circle).
      float radius;
      /// Width of the body (if it's a Rectangle).
      float width;
      /// Height of the body (if it's a Rectangle).
      float height;
      /// Flag indicating whether the body is affected by gravity.
      bool affectedByGravity;
      /// Mass of the rigidbody
      Engine::Property<float> mass{
        0, [this] {
          if (this->mass < 0.0f)
            this->mass = 0;
          else if (this->mass == 0.0f)
            this->massInv = 0;
          else
            this->massInv = 1.0f / this->mass;
        }
      };
      /// The coefficient of the frictional force which acts between the two surfaces when they are in the rest position
      /// with respect to each other.
      float staticFriction;
      /// The coefficient of the frictional force which is created between any two surfaces when they are in a moving
      /// position.
      float dynamicFriction;

      /// If true, the rigidbody will bounce off the top of the viewport
      bool bindToViewportTop;
      /// If true, the rigidbody will bounce off the bottom of the viewport
      bool bindToViewportBottom;
      /// If true, the rigidbody will bounce off the left of the viewport
      bool bindToViewportLeft;
      /// If true, the rigidbody will bounce off the right of the viewport
      bool bindToViewportRight;

      /// Default constructor.
      Rigidbody2D() = default;

      /**
       * Applies a force to the rigid body.
       * @param force The force vector to apply.
       */
      void AddForce(const Vector2f &force);
      /// @return The Rigidbody2DType of the body.
      [[nodiscard]] Rigidbody2DType Type() const;
      /// @returns The points at which this rigidbody collided with another rigidbody
      [[nodiscard]] std::vector<Vector2f> ContactPoints() const;
      /// Makes this rigidbody bounce off every part of the viewport
      void BindToViewport();
      /// Makes this rigidbody not bounce off every part of the viewport
      void UnbindFromViewport();
      /// Sets the friction coefficients to 0
      void NoFriction();
      /// Sets the friction coefficients to there default values:
      ///  - static: 0.6f
      ///  - dynamic: 0.4f
      void DefaultFriction();

      /**
       * Creates a circular rigid body.
       * @param radius The radius of the circle.
       * @param mass The density of the body.
       * @param isStatic Whether the body is static.
       * @return A Rigidbody2D instance representing the circular body.
       */
      static Rigidbody2D *CreateCircleBody(float radius, float mass, bool isStatic);
      /**
       * Creates a rectangular rigid body.
       * @param width The width of the rectangle.
       * @param height The height of the rectangle.
       * @param mass The density of the body.
       * @param isStatic Whether the body is static.
       * @return A Rigidbody2D instance representing the rectangular body.
       */
      static Rigidbody2D *CreateRectangleBody(
        float width, float height, float mass, bool isStatic
      );
    private:
      bool initialized;
      /// Angular velocity of the body.
      float angularVelocity;
      /// Inverse of the mass (cached to only use the division when mass changes)
      float massInv;
      /// Inertia of the rigidbody
      float inertia;
      /// Inverse of the inertia (cached to only use the division when inertia changes)
      float inertiaInv;
      /// Area of the rigid body.
      float area;
      /// Type of the rigid body (Circle or Rectangle).
      Rigidbody2DType type;
      /// Linear velocity of the rigid body.
      Vector2f linearVelocity;
      /// Accumulated force applied to the rigid body.
      Vector2f force;
      /// The axis-aligned bounding box  of this rigidbody.
      AABB aabb;
      /// Vertices of the rigid body (used for rectangles).
      std::vector<Vector2f> transformedVertices;
      /// The points at which this rigidbody collided with another rigidbody
      std::vector<Vector2f> contactPoints;
      /// The model matrix of the rigidbody last time it changed
      glm::mat4 lastModelMatrix;

      Rigidbody2D(
        float mass, float inertia, float area, bool isStatic, float radius, float width, float height, Rigidbody2DType type
      );

      /** @return A std::array containing the min and max points of the AABB. */
      [[nodiscard]] AABB getAABB();
      /// Simulates a step of the physics simulation for the current rigidbody
      void step(float fixedDeltaTime);
  };
}

#endif //RIGIDBODY2D_H
