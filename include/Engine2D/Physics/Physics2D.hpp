//
// Physics2D.h
// Author: Antoine Bastide
// Date: 29/11/2024
//

#ifndef PHYSICS2D_H
#define PHYSICS2D_H

#include "Engine2D/Physics/CollisionGrid.hpp"
#include "Engine2D/Physics/CollisionManifold.hpp"

namespace Engine {
  class Settings;
}

namespace Engine2D {
  class Game2D;
  class Entity2D;
}

namespace Engine2D::Physics {
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
}

template<>
struct std::hash<ContactPair> {
  std::size_t operator()(const ContactPair& pair) const noexcept {
    const auto h1 = reinterpret_cast<std::uintptr_t>(pair.collider1.get());
    const auto h2 = reinterpret_cast<std::uintptr_t>(pair.collider2.get());
    // Use a fast bitwise combination
    return h1 ^ h2 << 1;
  }
};


namespace Engine2D::Physics {
  class Physics2D {
    friend class Collider2D;
    friend class CollisionGrid;
    friend class Engine::Settings;
    friend class Engine2D::Game2D;

    enum CollisionEventType {
      Stay, Enter, Exit
    };

    static bool initialized;
    /// If the collision grid needs to be resized
    static bool collisionGridNeedsResizing;
    /// The spacial partition of the screen for collisions
    static CollisionGrid collisionGrid;

    /// The list of all the colliders that are in the game
    static std::vector<std::shared_ptr<Collider2D>> colliders;
    /// The list of colliders to add to the future physics simulations steps
    static std::vector<std::shared_ptr<Collider2D>> collidersToAdd;
    /// The list of colliders to remove from the future physics simulations steps
    static std::unordered_set<std::shared_ptr<Collider2D>> collidersToRemove;
    /// The list of currently active rigidbodies
    static std::vector<std::shared_ptr<Collider2D>> activeColliders;

    /// The list of pairs of rigidbodies that had a contact during this frame
    static std::vector<ContactPair> contactPairs;
    /// The list of pairs of rigidbodies that collided last simulation
    static std::vector<ContactPair> previousCollisionPairs;

    ~Physics2D();

    /// Adds a collider to the physics simulation
    static void addCollider(const std::shared_ptr<Collider2D> &collider);
    /// Adds all the colliders scheduled to be added to the simulation
    static void addColliders();
    /// Removes a collider to the physics simulation
    static void removeCollider(const std::shared_ptr<Collider2D> &collider);
    /// Removes all the colliders scheduled to be removed to the simulation
    static void removeColliders();

    /// Simulates a step of the physics simulation
    static void step();
    /// Filters all the colliders in the game and keeps only the ones that are active
    static void findActiveColliders();
    ///
    static void notifyCollisions(
      const std::shared_ptr<Collider2D> &sender, const std::shared_ptr<Collider2D> &receiver, CollisionEventType eventType
    );
    /// Collision detection
    static void broadPhase(const std::vector<std::shared_ptr<Collider2D>> &collidersToChecks);
    /// Collision resolution
    static void narrowPhase();
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
