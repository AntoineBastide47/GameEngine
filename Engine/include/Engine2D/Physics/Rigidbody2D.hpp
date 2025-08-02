//
// Rigidbody2D.h
// Author: Antoine Bastide
// Date: 30/11/2024
//

#ifndef RIGIDBODY2D_H
#define RIGIDBODY2D_H

#include <glm/glm.hpp>

#include "Engine2D/Component2D.hpp"
#include "Engine/Types/float01.hpp"
#include "Rigidbody2D.gen.hpp"

namespace Engine2D {
  class Entity2D;
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
  class Rigidbody2D final : public Component2D {
    SERIALIZE_RIGIDBODY2D
      friend class Physics2D;
      friend class Engine2D::Entity2D;
    public:
      /// Flag indicating whether the body is affected by forces.
      bool isKinematic;
      /// Flag indicating whether the body is affected by gravity.
      bool affectedByGravity;
      /// Linear velocity of the rigid body.
      glm::vec2 linearVelocity;
      /// Angular velocity of the body.
      float angularVelocity;
      /// Indicates how much of the angular velocity will be applied during collisions.
      /// For full rotation set it to 1, for no rotation set it to 0 or values in between for different reactions.
      Engine::float01 angularDamping;
      /// The coefficient of the frictional force which acts between the two surfaces when they are in the rest position
      /// with respect to each other.
      float staticFriction;
      /// The coefficient of the frictional force which is created between any two surfaces when they are in a moving
      /// position.
      float dynamicFriction;

      /// Sets the mass of this rigidbody
      void SetMass(float mass);
      /// @returns The mass of this rigidbody
      [[nodiscard]] float GetMass() const;

      /**
       * Applies a force to the rigid body.
       * @param force The force vector to apply.
       */
      void AddForce(const glm::vec2 &force);
      /// Sets the friction coefficients to 0
      void NoFriction();
      /// Sets the friction coefficients to there default values:
      ///  - static: 0.6f
      ///  - dynamic: 0.4f
      void DefaultFriction();

      void OnDeserialize(Engine::Reflection::Format format, const Engine::JSON &json) override;
    private:
      /// Mass of the rigidbody
      ENGINE_SERIALIZE float mass;
      /// Inverse of the mass (cached to only use the division when mass changes)
      float massInv;
      /// Inertia of the rigidbody
      ENGINE_SERIALIZE float inertia;
      /// Inverse of the inertia (cached to only use the division when inertia changes)
      float inertiaInv;
      /// Accumulated force applied to the rigidbody.
      glm::vec2 force;

      static constexpr float RECTANGLE_COLLISION_FACTOR = 1.0f / 12.0f;

      Rigidbody2D();

      /// Calculates the inertia of this body using the given collider
      void computeInertia(const Collider2D *collider);
      /// Runs a physics step on this rigidbody
      void step();
  };
}

#endif //RIGIDBODY2D_H
