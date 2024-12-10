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
    public:
      enum Rigidbody2DType {
        Circle, Rectangle
      };

      /// Density of the rigid body.
      float density{};
      /// Coefficient of restitution (bounciness) of the rigid body.
      float restitution{};
      /// Flag indicating whether the body is static (immovable).
      bool isStatic{};
      /// Radius of the body (if it's a Circle).
      float radius{};
      /// Width of the body (if it's a Rectangle).
      float width{};
      /// Height of the body (if it's a Rectangle).
      float height{};
      /// Flag indicating whether the body is affected by gravity.
      bool affectedByGravity{};
      /// Mass of the rigidbody
      Engine::Property<float> mass = Engine::Property<float>{
        0, [this] {
          if (this->mass < 0.0f)
            this->mass = 0.0f;
          else if (this->mass == 0.0f)
            this->massInv = 0.0f;
          else
            this->massInv = 1.0f / this->mass;
        }
      };

      /// Default constructor.
      Rigidbody2D() = default;

      /**
       * Applies a force to the rigid body.
       * @param force The force vector to apply.
       */
      void AddForce(const Vector2 &force);

      /**
       * Gets the type of the rigid body (Circle or Rectangle).
       * @return The Rigidbody2DType of the body.
       */
      [[nodiscard]] Rigidbody2DType Type() const;
      /** @return A vector containing the vertices of the body. */
      [[nodiscard]] std::vector<Vector2> Vertices();
      /** @return A std::array containing the min and max points of the AABB. */
      [[nodiscard]] std::array<Vector2, 2> GetAABB();

      /**
       * Creates a circular rigid body.
       * @param radius The radius of the circle.
       * @param mass The mass of the body.
       * @param density The density of the body.
       * @param restitution The coefficient of restitution.
       * @param isStatic Whether the body is static.
       * @return A Rigidbody2D instance representing the circular body.
       */
      static Rigidbody2D CreateCircleBody(float radius, float mass, float density, float restitution, bool isStatic);
      /**
       * Creates a rectangular rigid body.
       * @param width The width of the rectangle.
       * @param height The height of the rectangle.
       * @param mass The mass of the body.
       * @param density The density of the body.
       * @param restitution The coefficient of restitution.
       * @param isStatic Whether the body is static.
       * @return A Rigidbody2D instance representing the rectangular body.
       */
      static Rigidbody2D CreateRectangleBody(
        float width, float height, float mass, float density, float restitution, bool isStatic
      );
    private:
      /// Rotational velocity of the body.
      float rotationalVelocity{};
      /// Inverse of the mass (for fast calculations).
      float massInv{};
      /// Area of the rigid body.
      float area{};
      /// Type of the rigid body (Circle or Rectangle).
      Rigidbody2DType type{};
      /// Linear velocity of the rigid body.
      Vector2 linearVelocity;
      /// Accumulated force applied to the rigid body.
      Vector2 force;
      /// Minimum point of the axis-aligned bounding box (AABB).
      Vector2 aabbMin;
      /// Maximum point of the axis-aligned bounding box (AABB).
      Vector2 aabbMax;
      /// Vertices of the rigid body (used for rectangles).
      std::vector<Vector2> vertices;

      /**
       * Private constructor for initializing rigid bodies with detailed parameters.
       * @param density The density of the rigid body.
       * @param mass The mass of the rigid body.
       * @param restitution The coefficient of restitution.
       * @param area The area of the rigid body.
       * @param isStatic Whether the body is static.
       * @param radius The radius (if it's a Circle).
       * @param width The width (if it's a Rectangle).
       * @param height The height (if it's a Rectangle).
       * @param type The type of the rigid body (Circle or Rectangle).
       */
      Rigidbody2D(
        float density, float mass, float restitution, float area, bool isStatic, float radius, float width, float height,
        Rigidbody2DType type
      );

      /**
       * Simulates the motion of the rigid body for a single physics step.
       * @param fixedDeltaTime The time step duration.
       */
      void Step(float fixedDeltaTime);
  };
}

#endif //RIGIDBODY2D_H
