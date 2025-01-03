//
// Physics2D.cpp
// Author: Antoine Bastide
// Date: 29/11/2024
//

#include <ranges>

#include "2D/Physics/Physics2D.h"
#include "2D/Entity2D.h"
#include "2D/Game2D.h"
#include "2D/Physics/Collisions.h"
#include "2D/Physics/Rigidbody2D.h"
#include "Common/Settings.h"

namespace Engine2D::Physics {
  Physics2D::Physics2D() : collisionGrid(Engine::Settings::Physics::GetPartitionSize()), collisionGridNeedsResizing(false) {}

  Physics2D::~Physics2D() {
    rigidbodies.clear();
    rigidbodiesToRemove.clear();
  }

  void Physics2D::addRigidBody(const std::shared_ptr<Rigidbody2D> &rigidbody) {
    if (rigidbody)
      rigidbodiesToAdd.insert(rigidbody);
  }

  void Physics2D::addRigidbodies() {
    // Add all the new rigidbodies to the physics world
    if (!rigidbodiesToAdd.empty())
      for (auto rigidbody: rigidbodiesToAdd)
        rigidbodies.insert(rigidbody);
    rigidbodiesToAdd.clear();
  }

  void Physics2D::removeRigidbody(const std::shared_ptr<Rigidbody2D> &rigidbody) {
    if (rigidbody)
      rigidbodiesToRemove.insert(rigidbody);
  }

  void Physics2D::removeRigidbodies() {
    // Remove all the rigidbodies that no longer need to be simulated
    if (!rigidbodiesToRemove.empty()) {
      for (const std::shared_ptr rigidbody: rigidbodiesToRemove)
        if (auto it = std::ranges::find(rigidbodies, rigidbody); it != rigidbodies.end())
          rigidbodies.erase(it);
      rigidbodiesToRemove.clear();
    }
  }

  void Physics2D::step() {
    addRigidbodies();

    // Early out if there are no physics components
    if (rigidbodies.empty())
      return;

    simulate();
    // Skip the collision detection if there are no active colliders
    findActiveColliders();
    if (!activeColliders.empty()) {
      if (!Engine::Settings::Physics::GetUseScreenPartitioning())
        broadPhase();
      else {
        if (collisionGridNeedsResizing)
          collisionGrid.setGridSize(Engine::Settings::Physics.GetPartitionSize());
        collisionGrid.update(activeRigidbodies);
        contactPairs = collisionGrid.collisionPairs();
      }
      narrowPhase();
      activeRigidbodies.clear();
      contactPairs.clear();
    }

    removeRigidbodies();
  }

  void Physics2D::simulate() {
    // Simulate a step for each rigidbody, check for null pointers and get all the active rigidbodies
    bool foundNull = false;
    const float fixedDeltaTime = Engine::Settings::Physics::GetFixedDeltaTime();
    for (const auto rigidbody: rigidbodies) {
      if (!rigidbody)
        foundNull = true;
      else if (rigidbody->active && rigidbody->Entity() && rigidbody->Entity()->active)
        activeRigidbodies.push_back(rigidbody);
    }

    const float fixedDeltaTime = Engine::Settings::Physics.GetFixedDeltaTime();
    for (const auto rigidbody: activeRigidbodies)
      if (!rigidbody->isStatic)
        rigidbody->step(fixedDeltaTime);

    // Remove all the null pointers if at least one is found
    if (foundNull)
      for (auto it = rigidbodies.begin(); it != rigidbodies.end();) {
        if (*it == nullptr)
          it = rigidbodies.erase(it);
        else
          ++it;
      }
  }

  void Physics2D::broadPhase() {
    // Check collisions for all the rigidbodies that are active
    for (int i = 0; i < activeRigidbodies.size() - 1; i++) {
      std::shared_ptr<Rigidbody2D> rb1 = activeRigidbodies[i];
      const Rigidbody2D::AABB rb1AABB = rb1->getAABB();
      for (int j = i + 1; j < activeRigidbodies.size(); j++) {
        std::shared_ptr<Rigidbody2D> rb2 = activeRigidbodies[j];
        // Do nothing if both colliders are static
        if (rb1->isStatic && rb2->isStatic)
          continue;

        // Cheap collision check for early out
        if (!Collisions::collideAABB(rb1AABB, rb2->getAABB()))
          continue;

        // Save the indices of the bodies that collided
        contactPairs.push_back({rb1, rb2});
      }
    }
  }

  void Physics2D::narrowPhase() {
    for (auto [rb1, rb2]: contactPairs) {
      // More accurate SAT collision check
      Vector2f normal;
      double depth = std::numeric_limits<double>::max();
      if (Collisions::collide(rb1, rb2, &normal, &depth)) {
        // Move the bodies
        separateBodies(rb1, rb2, normal * depth);

        // Find contact points
        Vector2f contactPoint1, contactPoint2;
        uint8_t contactCount = 0;
        Collisions::findContactPoints(rb1, rb2, &contactPoint1, &contactPoint2, &contactCount);

        // Resolve the collision by inverting the normal to make it point outwards
        Engine::Physics::CollisionManifold contact{rb1, rb2, -normal, depth, contactPoint1, contactPoint2, contactCount};
        resolveCollision(contact);

        // Notify the callbacks
        contact.rb1->contactPoints = contact.contactPoints;
        contact.rb1->Entity()->OnCollision(contact.rb2);
        contact.rb2->contactPoints = contact.contactPoints;
        contact.rb2->Entity()->OnCollision(contact.rb1);
      }
    }
  }

  void Physics2D::separateBodies(
    const std::shared_ptr<Rigidbody2D> &rb1, const std::shared_ptr<Rigidbody2D> &rb2, const Vector2f mtv
  ) {
    if (rb1->isStatic)
      rb2->Transform()->position -= mtv;
    else if (rb2->isStatic)
      rb1->Transform()->position += mtv;
    else {
      rb1->Transform()->position -= mtv * 0.5f;
      rb2->Transform()->position += mtv * 0.5f;
    }
  }

  void Physics2D::resolveCollision(const Engine::Physics::CollisionManifold &contact) {
    const float bounciness = std::min(contact.rb1->restitution, contact.rb2->restitution);
    std::array<Vector2f, 2> impulses;
    std::array<float, 2> jList;

    // Find the impulses to apply
    for (int i = 0; i < contact.contactPoints.size(); i++) {
      const Vector2f raPerp = (contact.contactPoints[i] - contact.rb1->Transform()->WorldPosition()).Perpendicular();
      const Vector2f rbPerp = (contact.contactPoints[i] - contact.rb2->Transform()->WorldPosition()).Perpendicular();

      // Find the relative velocity
      const Vector2f angularLinearVelocityA = raPerp * contact.rb1->angularVelocity;
      const Vector2f angularLinearVelocityB = rbPerp * contact.rb2->angularVelocity;
      const Vector2f relativeVelocity = contact.rb2->linearVelocity + angularLinearVelocityB -
                                        (contact.rb1->linearVelocity + angularLinearVelocityA);

      if (const float contactVelocityMag = relativeVelocity * contact.normal; contactVelocityMag <= 0.0f) {
        // Compute the impulse
        const float raPerpMag = raPerp * contact.normal;
        const float rbPerpMag = rbPerp * contact.normal;
        const float denominator = contact.rb1->massInv + contact.rb2->massInv +
                                  raPerpMag * raPerpMag * contact.rb1->inertiaInv +
                                  rbPerpMag * rbPerpMag * contact.rb2->inertiaInv;
        const float j = -(1 + bounciness) * contactVelocityMag / (denominator * contact.contactPoints.size());
        jList[i] = j;
        impulses[i] = j * contact.normal;
      }
    }

    // Apply the impulses to update the linear and angular velocity
    for (int i = 0; i < impulses.size(); i++) {
      const Vector2f ra = contact.contactPoints[i] - contact.rb1->Transform()->WorldPosition();
      const Vector2f rb = contact.contactPoints[i] - contact.rb2->Transform()->WorldPosition();

      contact.rb1->linearVelocity -= impulses[i] * contact.rb1->massInv;
      contact.rb1->angularVelocity -= impulses[i] ^ ra * contact.rb1->inertiaInv;
      contact.rb2->linearVelocity += impulses[i] * contact.rb2->massInv;
      contact.rb2->angularVelocity += impulses[i] ^ rb * contact.rb2->inertiaInv;
    }

    if (Engine::Settings::Physics.GetFrictionEnabled()) {
      const float sf = (contact.rb1->staticFriction + contact.rb2->staticFriction) * 0.5f;
      const float df = (contact.rb1->dynamicFriction + contact.rb2->dynamicFriction) * 0.5f;
      std::array<Vector2f, 2> frictionImpulses;

      // Find the friction impulses to apply
      for (int i = 0; i < contact.contactPoints.size(); i++) {
        const Vector2f raPerp = (contact.contactPoints[i] - contact.rb1->Transform()->WorldPosition()).Perpendicular();
        const Vector2f rbPerp = (contact.contactPoints[i] - contact.rb2->Transform()->WorldPosition()).Perpendicular();

        // Find the relative velocity
        const Vector2f angularLinearVelocityA = raPerp * contact.rb1->angularVelocity;
        const Vector2f angularLinearVelocityB = rbPerp * contact.rb2->angularVelocity;
        const Vector2f relativeVelocity = contact.rb2->linearVelocity + angularLinearVelocityB -
                                          (contact.rb1->linearVelocity + angularLinearVelocityA);

        Vector2f tangent = relativeVelocity - relativeVelocity * contact.normal * contact.normal;
        if (Vector2f::ApproxEquals(tangent, Vector2f::Zero))
          continue;
        tangent.Normalize();

        // Compute the impulse
        const float raPerpMag = raPerp * tangent;
        const float rbPerpMag = rbPerp * tangent;
        const float denominator = contact.rb1->massInv + contact.rb2->massInv +
                                  raPerpMag * raPerpMag * contact.rb1->inertiaInv +
                                  rbPerpMag * rbPerpMag * contact.rb2->inertiaInv;
        if (const float jt = -relativeVelocity * tangent / (denominator * contact.contactPoints.size());
          std::abs(jt) <= jList[i] * sf)
          frictionImpulses[i] = jt * contact.normal;
        else
          frictionImpulses[i] = -jt * contact.normal * df;
      }

      // Apply the friction impulses to update the linear and angular velocity
      for (int i = 0; i < frictionImpulses.size(); i++) {
        const Vector2f ra = contact.contactPoints[i] - contact.rb1->Transform()->WorldPosition();
        const Vector2f rb = contact.contactPoints[i] - contact.rb2->Transform()->WorldPosition();

        contact.rb1->linearVelocity -= frictionImpulses[i] * contact.rb1->massInv;
        contact.rb1->angularVelocity -= frictionImpulses[i] ^ ra * contact.rb1->inertiaInv;
        contact.rb2->linearVelocity += frictionImpulses[i] * contact.rb2->massInv;
        contact.rb2->angularVelocity += frictionImpulses[i] ^ rb * contact.rb2->inertiaInv;
      }
    }
  }
}
