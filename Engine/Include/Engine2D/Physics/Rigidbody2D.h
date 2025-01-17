//
// Rigidbody2D.h
// Author: Antoine Bastide
// Date: 30/11/2024
//

#ifndef RIGIDBODY2D_H
#define RIGIDBODY2D_H

#include "Engine2D/Component2D.h"
#include "Engine2D/Types/Vector2.h"
#include "Common/Property.h"
#include "Types/float01.h"

namespace Engine2D {
  class Game2D;
}

namespace Engine2D::Physics {
  class Collider2D;
  /**
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
      /// Flag indicating whether the body is affected by forces.
      bool isKinematic;
      /// Flag indicating whether the body is affected by gravity.
      bool affectedByGravity;
      /// Mass of the rigidbody
      Engine::Property<float> mass;
      /// Linear velocity of the rigid body.
      Vector2f linearVelocity;
      /// Angular velocity of the body.
      float angularVelocity;
      /// Indicates how much of the angular velocity will be applied during collisions.
      /// For full rotation set it to 1, for no rotation set it to 0 or values inbetween for different reactions.
      Engine::float01 angularDamping;
      /// The coefficient of the frictional force which acts between the two surfaces when they are in the rest position
      /// with respect to each other.
      float staticFriction;
      /// The coefficient of the frictional force which is created between any two surfaces when they are in a moving
      /// position.
      float dynamicFriction;

      /// Default constructor.
      Rigidbody2D();

      /**
       * Applies a force to the rigid body.
       * @param force The force vector to apply.
       */
      void AddForce(const Vector2f &force);
      /// Sets the friction coefficients to 0
      void NoFriction();
      /// Sets the friction coefficients to there default values:
      ///  - static: 0.6f
      ///  - dynamic: 0.4f
      void DefaultFriction();
    private:
      /// Inverse of the mass (cached to only use the division when mass changes)
      float massInv;
      /// Inertia of the rigidbody
      float inertia;
      /// Inverse of the inertia (cached to only use the division when inertia changes)
      float inertiaInv;
      /// Accumulated force applied to the rigidbody.
      Vector2f force;

      /// Calculates the inertia of this body using the given collider
      void computeInertia(const std::shared_ptr<Collider2D> &collider);

      /// Simulates a step of the physics simulation for the current rigidbody
      void OnFixedUpdate() override;
  };
}

#endif //RIGIDBODY2D_H
