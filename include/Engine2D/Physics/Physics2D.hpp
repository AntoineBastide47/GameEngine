//
// Physics2D.h
// Author: Antoine Bastide
// Date: 29/11/2024
//

#ifndef PHYSICS2D_H
#define PHYSICS2D_H

#include "Engine2D/Physics/CollisionGrid.hpp"
#include "Engine2D/Physics/CollisionManifold.hpp"
#include "Engine2D/Physics/Collider2D.hpp"

namespace Engine {
  class Settings;
}

namespace Engine2D {
  class Game2D;
  class Entity2D;
}

namespace Engine2D::Physics {
  class Rigidbody2D;

  struct ContactPair {
    std::shared_ptr<Collider2D> collider1;
    std::shared_ptr<Collider2D> collider2;
    std::shared_ptr<Rigidbody2D> rigidbody1;
    std::shared_ptr<Rigidbody2D> rigidbody2;

    bool operator==(const ContactPair &other) const {
      return collider1 == other.collider1 && collider2 == other.collider2 &&
             rigidbody1 == other.rigidbody1 && rigidbody2 == other.rigidbody2;
    }
  };

  class Physics2D {
    friend class Engine2D::Entity2D;
    friend class Engine2D::Game2D;
    friend class Engine::Settings;
    friend class CollisionGrid;

    enum CollisionEventType {
      Stay, Enter, Exit
    };

    /// If the collision grid needs to be resized
    bool collisionGridNeedsResizing;
    /// The spacial partition of the screen for collisions
    CollisionGrid collisionGrid;

    /// The list of all the colliders that are in the game
    std::vector<std::shared_ptr<Collider2D>> colliders;
    /// The list of colliders to add to the future physics simulations steps
    std::vector<std::shared_ptr<Collider2D>> collidersToAdd;
    /// The list of colliders to remove from the future physics simulations steps
    std::vector<std::shared_ptr<Collider2D>> collidersToRemove;
    /// The list of currently active rigidbodies
    std::vector<std::shared_ptr<Collider2D>> activeColliders;

    /// The list of pairs of rigidbodies that had a contact during this frame
    std::vector<ContactPair> contactPairs;
    /// The list of pairs of rigidbodies that collided last simulation
    std::vector<ContactPair> previousCollisionPairs;

    /// Creates the physics world instance and sets the update rate
    Physics2D();
    ~Physics2D();

    /// Adds a collider to the physics simulation
    void addCollider(const std::shared_ptr<Collider2D> &collider);
    /// Adds all the colliders scheduled to be added to the simulation
    void addColliders();
    /// Removes a collider to the physics simulation
    void removeCollider(const std::shared_ptr<Collider2D> &collider);
    /// Removes all the colliders scheduled to be removed to the simulation
    void removeColliders();

    /// Simulates a step of the physics simulation
    void step();
    /// Filters all the colliders in the game and keeps only the ones that are active
    void findActiveColliders();
    static void notifyCollisions(
      const std::shared_ptr<Collider2D> &sender, const std::shared_ptr<Collider2D> &receiver, CollisionEventType eventType
    );
    /// Collision detection
    void broadPhase(const std::vector<std::shared_ptr<Collider2D>> &collidersToChecks);
    /// Collision resolution
    void narrowPhase();
    /// Separates the given bodies using the given Minimum Translation Vector to make sure they are not contained in each other
    static void separateBodies(
      const std::shared_ptr<Collider2D> &col1, const std::shared_ptr<Collider2D> &col2,
      const std::shared_ptr<Rigidbody2D> &rb1, const std::shared_ptr<Rigidbody2D> &rb2, glm::vec2 mtv
    );
    /// Resolves the collision between two rigidbodies
    static void resolveCollision(const Engine::Physics::CollisionManifold &contact);
  };
}

#endif //PHYSICS2D_H
