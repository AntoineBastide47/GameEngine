//
// Physics2D.h
// Author: Antoine Bastide
// Date: 29/11/2024
//

#ifndef PHYSICS2D_H
#define PHYSICS2D_H

#include <vector>

#include "CollisionGrid.h"
#include "CollisionManifold.h"
#include "2D/Types/Vector2.h"

namespace Engine {
  class Settings;
}

namespace Engine2D {
  class Entity2D;
  class Game2D;
}

namespace Engine2D::Physics {
  class Rigidbody2D;
  class Physics2D {
    friend class Engine2D::Entity2D;
    friend class Engine2D::Game2D;
    friend class Engine::Settings;

    /// The spacial partition of the screen for collisions
    CollisionGrid collisionGrid;
    /// If the collision grid needs to be resized
    bool collisionGridNeedsResizing;
    /// The list of all the rigidbodies that are in the game
    std::vector<Rigidbody2D *> rigidbodies;
    /// The list of rigidbodies to add to the future physics simulations steps
    std::vector<Rigidbody2D *> rigidbodiesToAdd;
    /// The list of rigidbodies to remove from the future physics simulations steps
    std::vector<Rigidbody2D *> rigidbodiesToRemove;
    /// The list of pairs of rigidbodies that collided
    std::vector<std::tuple<Rigidbody2D *, Rigidbody2D *> > contactPairs;
    /// The list of currently active rigidbodies
    std::vector<Rigidbody2D *> activeRigidbodies;

    /// Creates the physics world instance and sets the update rate
    Physics2D();
    ~Physics2D();

    /// Adds a rigidbody to the physics simulation
    void addRigidBody(Rigidbody2D *rigidbody);
    /// Adds all the rigidbodies scheduled to be added to the simulation
    void addRigidbodies();
    /// Removes a rigidbody to the physics simulation
    void removeRigidbody(Rigidbody2D *rigidbody);
    /// Removes all the rigidbodies scheduled to be removed to the simulation
    void removeRigidbodies();

    /// Simulates a step of the physics simulation
    void step();
    /// Simulates all the bodies of the simulation
    void simulate();
    /// Collision detection
    void broadPhase();
    /// Collision resolution
    void narrowPhase();
    /// Separates the given bodies using the given Minimum Translation Vector to make sure they are not contained in each other
    static void separateBodies(const Rigidbody2D *rb1, const Rigidbody2D *rb2, Vector2f mtv);
    /// Resolves the collision between two rigidbodies
    static void resolveCollision(const Engine::Physics::CollisionManifold &contact);
  };
}

#endif //PHYSICS2D_H
