//
// Physics2D.cpp
// Author: Antoine Bastide
// Date: 29/11/2024
//

#include <ranges>

#include "Engine2D/Physics/Physics2D.h"
#include "Engine2D/Entity2D.h"
#include "Engine2D/Game2D.h"
#include "Engine2D/Physics/Collider2D.h"
#include "Engine2D/Physics/Collisions.h"
#include "Engine2D/Physics/Rigidbody2D.h"
#include "Common/Settings.h"

namespace Engine2D::Physics {
  Physics2D::Physics2D() : collisionGridNeedsResizing(false), collisionGrid(Engine::Settings::Physics::GetPartitionSize()) {}

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

  void Physics2D::addCollider(const std::shared_ptr<Collider2D> &collider) {
    if (collider)
      collidersToAdd.insert(collider);
  }

  void Physics2D::addColliders() {
    if (!collidersToAdd.empty())
      for (auto collider: collidersToAdd)
        colliders.insert(collider);
    collidersToAdd.clear();
  }

  void Physics2D::removeCollider(const std::shared_ptr<Collider2D> &collider) {
    if (collider)
      collidersToRemove.insert(collider);
  }

  void Physics2D::removeColliders() {
    if (!collidersToRemove.empty())
      for (auto collider: collidersToRemove)
        colliders.erase(collider);
    collidersToRemove.clear();
  }

  void Physics2D::step() {
    // Update the physics components
    addRigidbodies();
    removeRigidbodies();
    addColliders();
    removeColliders();

    // Skip the collision detection if there are no active colliders
    findActiveColliders();
    if (!activeColliders.empty()) {
      if (Engine::Settings::Physics::GetUseScreenPartitioning()) {
        // Update the collision grid
        if (collisionGridNeedsResizing)
          collisionGrid.setGridSize(Engine::Settings::Physics::GetPartitionSize());
        collisionGrid.update(activeColliders);
        // Perform the broad phase for each cell of the grid
        for (auto &gridCol: collisionGrid.grid)
          for (auto &gridCell: gridCol)
            if (!gridCell.empty())
              broadPhase(gridCell);
      } else
        broadPhase(activeColliders);
      narrowPhase();

      // Delete old data
      activeColliders.clear();
      contactPairs.clear();
    }

    // Early out if there are no physics components
    if (rigidbodies.empty())
      return;
    simulate();
  }

  void Physics2D::simulate() {
    // Simulate a step for each rigidbody, check for null pointers and get all the active rigidbodies
    bool foundNull = false;
    const float fixedDeltaTime = Engine::Settings::Physics::GetFixedDeltaTime();
    for (const auto rigidbody: rigidbodies) {
      if (rigidbody && rigidbody->IsActive() && !rigidbody->isKinematic)
        rigidbody->step(fixedDeltaTime);
      else if (!rigidbody)
        foundNull = true;
    }

    // Remove all the null pointers if at least one is found
    if (foundNull)
      for (auto it = rigidbodies.begin(); it != rigidbodies.end();) {
        if (*it == nullptr)
          it = rigidbodies.erase(it);
        else
          ++it;
      }
  }

  void Physics2D::findActiveColliders() {
    // Find all the active colliders
    bool foundNull = false;
    for (const auto collider: colliders) {
      if (collider && collider->IsActive() && collider->type != Collider2D::None)
        activeColliders.push_back(collider);
      else if (!collider)
        foundNull = true;
    }

    // Remove all the null pointers if at least one is found
    if (foundNull)
      for (auto it = colliders.begin(); it != colliders.end();) {
        if (*it == nullptr)
          it = colliders.erase(it);
        else
          ++it;
      }
  }

  void Physics2D::notifyCollisions(
    const std::shared_ptr<Collider2D> &sender, const std::shared_ptr<Collider2D> &receiver,
    const CollisionEventType eventType
  ) {
    if (sender->isTrigger) {
      switch (eventType) {
        case Stay: sender->Entity()->OnTriggerStay2D(receiver);
          break;
        case Enter: sender->Entity()->OnTriggerEnter2D(receiver);
          break;
        case Exit: sender->Entity()->OnTriggerExit2D(receiver);
          break;
      }
    } else {
      switch (eventType) {
        case Stay: sender->Entity()->OnCollisionStay2D(receiver);
          break;
        case Enter: sender->Entity()->OnCollisionEnter2D(receiver);
          break;
        case Exit: sender->Entity()->OnCollisionExit2D(receiver);
          break;
      }
    }
  }

  void Physics2D::broadPhase(const std::vector<std::shared_ptr<Collider2D>> &collidersToChecks) {
    // Check collisions for all the rigidbodies that are active
    for (int i = 0; i < collidersToChecks.size() - 1; i++) {
      const auto col1 = collidersToChecks[i];
      const auto col1AABB = col1->getAABB();
      for (int j = i + 1; j < collidersToChecks.size(); j++) {
        const auto col2 = collidersToChecks[j];

        const auto rb1 = col1->rigidbody;
        const auto rb2 = col2->rigidbody;
        // Create the contact pair using pointer address for ordering to ensure consistency
        ContactPair contactPair{
          col1 < col2 ? col1 : col2,
          col1 < col2 ? col2 : col1,
          col1 < col2 ? rb1 : rb2,
          col1 < col2 ? rb2 : rb1,
        };
        // Make sure it is not a collision between two static colliders, prevent entities colliding with themselves
        // and do a cheap collision check for early out
        if ((!rb1 && !rb2) || col1->Entity() == col2->Entity() || !Collisions::collideAABB(col1AABB, col2->getAABB())) {
          if (previousCollisionPairs.contains(contactPair)) {
            notifyCollisions(col1, col2, Exit);
            notifyCollisions(col2, col1, Exit);
          }

          continue;
        }

        // Save the indices of the bodies that collided using Canonical ordering
        contactPairs.insert(contactPair);
      }
    }
  }

  void Physics2D::narrowPhase() {
    if (contactPairs.empty()) {
      previousCollisionPairs.clear();
      return;
    }

    std::unordered_set<ContactPair> collisionPairs;
    for (auto &[col1, col2, rb1, rb2]: contactPairs) {
      // More accurate SAT collision check
      Vector2f normal;
      double depth = std::numeric_limits<double>::max();
      if (Collisions::collide(col1, col2, &normal, &depth)) {
        separateBodies(col1, col2, rb1, rb2, normal * depth);

        // Find contact points
        Vector2f contactPoint1, contactPoint2;
        uint8_t contactCount = 0;
        Collisions::findContactPoints(col1, col2, &contactPoint1, &contactPoint2, &contactCount);

        // Resolve the collision by inverting the normal to make it point outwards
        Engine::Physics::CollisionManifold contact{
          col1, col2, rb1, rb2, -normal, contactPoint1, contactPoint2, contactCount
        };
        resolveCollision(contact);
        col1->contactPoints = contact.contactPoints;
        col2->contactPoints = contact.contactPoints;

        // Notify the entities of the collisions
        ContactPair collisionPair{col1, col2, rb1, rb2};
        if (previousCollisionPairs.contains(collisionPair)) {
          notifyCollisions(col1, col2, Stay);
          notifyCollisions(col2, col1, Stay);
        } else {
          notifyCollisions(col1, col2, Enter);
          notifyCollisions(col2, col1, Enter);
        }

        collisionPairs.insert(collisionPair);
      }

      // Delete the previous collision pairs and replace them by the new ones
      previousCollisionPairs = std::move(collisionPairs);
    }
  }

  void Physics2D::separateBodies(
    const std::shared_ptr<Collider2D> &col1, const std::shared_ptr<Collider2D> &col2,
    const std::shared_ptr<Rigidbody2D> &rb1, const std::shared_ptr<Rigidbody2D> &rb2, const Vector2f mtv
  ) {
    const bool separateBody1 = rb1 && !rb1->isKinematic && !col1->isTrigger;
    const bool separateBody2 = rb2 && !rb2->isKinematic && !col2->isTrigger;
    const float invMass1 = separateBody1 ? rb1->massInv : 0.0f;
    const float invMass2 = separateBody2 ? rb2->massInv : 0.0f;
    const float ratio1 = invMass1 / (invMass1 + invMass2);
    const float ratio2 = invMass2 / (invMass1 + invMass2);

    if (separateBody1 && !separateBody2)
      col1->Transform()->position += ratio1 * mtv;
    else if (!separateBody1 && separateBody2)
      col2->Transform()->position -= ratio2 * mtv;
    else {
      col1->Transform()->position += ratio1 * mtv * 0.5f;
      col2->Transform()->position -= ratio2 * mtv * 0.5f;
    }
  }

  void Physics2D::resolveCollision(const Engine::Physics::CollisionManifold &contact) {
    const float bounciness = std::min(contact.col1->bounciness, contact.col2->bounciness);
    std::array<Vector2f, 2> impulses;
    std::array<float, 2> jList;
    const bool rb1NotStaticOrKinematic = !(!contact.rb1 || (contact.rb1 && contact.rb1->isKinematic));
    const bool rb2NotStaticOrKinematic = !(!contact.rb2 || (contact.rb2 && contact.rb2->isKinematic));

    if (rb1NotStaticOrKinematic)
      contact.rb1->computeInertia(contact.col1);
    if (rb2NotStaticOrKinematic)
      contact.rb2->computeInertia(contact.col2);

    // Find the impulses to apply
    for (int i = 0; i < contact.contactPoints.size(); i++) {
      const Vector2f raPerp = (contact.contactPoints[i] - contact.col1->Transform()->WorldPosition()).Perpendicular();
      const Vector2f rbPerp = (contact.contactPoints[i] - contact.col2->Transform()->WorldPosition()).Perpendicular();

      // Find the relative velocity
      const auto velA = contact.rb1 ? raPerp * contact.rb1->angularVelocity + contact.rb1->linearVelocity : Vector2f::Zero;
      const auto velB = contact.rb2 ? rbPerp * contact.rb2->angularVelocity + contact.rb2->linearVelocity : Vector2f::Zero;
      const Vector2f relativeVelocity = velB - velA;

      if (const float contactVelocityMag = relativeVelocity * contact.normal; contactVelocityMag <= 0.0f) {
        // Compute the impulse
        const float raPerpMag = raPerp * contact.normal;
        const float rbPerpMag = rbPerp * contact.normal;
        const float rb1Denom = rb1NotStaticOrKinematic
                                 ? contact.rb1->massInv + raPerpMag * raPerpMag * contact.rb1->inertiaInv
                                 : 0.0f;
        const float rb2Denom = rb2NotStaticOrKinematic
                                 ? contact.rb2->massInv + rbPerpMag * rbPerpMag * contact.rb2->inertiaInv
                                 : 0.0f;
        float j = -(1 + bounciness) * contactVelocityMag;
        if (const float denominator = rb1Denom + rb2Denom; denominator > 0.0f)
          j /= denominator * contact.contactPoints.size();
        jList[i] = j;
        impulses[i] = j * contact.normal;
      }
    }

    // Apply the impulses to update the linear and angular velocity
    for (int i = 0; i < impulses.size(); i++) {
      if (rb1NotStaticOrKinematic) {
        const Vector2f ra = contact.contactPoints[i] - contact.col1->Transform()->WorldPosition();
        contact.rb1->linearVelocity -= impulses[i] * contact.rb1->massInv;
        contact.rb1->angularVelocity -= impulses[i] ^ ra * contact.rb1->inertiaInv;
      }
      if (rb2NotStaticOrKinematic) {
        const Vector2f rb = contact.contactPoints[i] - contact.col2->Transform()->WorldPosition();
        contact.rb2->linearVelocity += impulses[i] * contact.rb2->massInv;
        contact.rb2->angularVelocity += impulses[i] ^ rb * contact.rb2->inertiaInv;
      }
    }

    if (Engine::Settings::Physics::GetFrictionEnabled()) {
      float sf = 0.0f;
      float df = 0.0f;
      if (contact.rb1) {
        sf += contact.rb1->staticFriction;
        df += contact.rb1->dynamicFriction;
      }
      if (contact.rb2) {
        sf += contact.rb2->staticFriction;
        df += contact.rb2->dynamicFriction;
      }
      sf *= 0.5f;
      df *= 0.5f;
      std::array<Vector2f, 2> frictionImpulses;

      // Find the friction impulses to apply
      for (int i = 0; i < contact.contactPoints.size(); i++) {
        const Vector2f raPerp = (contact.contactPoints[i] - contact.col1->Transform()->WorldPosition()).Perpendicular();
        const Vector2f rbPerp = (contact.contactPoints[i] - contact.col2->Transform()->WorldPosition()).Perpendicular();

        // Find the relative velocity
        const auto velA = rb1NotStaticOrKinematic
                            ? raPerp * contact.rb1->angularVelocity + contact.rb1->linearVelocity
                            : Vector2f::Zero;
        const auto velB = rb2NotStaticOrKinematic
                            ? rbPerp * contact.rb2->angularVelocity + contact.rb2->linearVelocity
                            : Vector2f::Zero;
        const Vector2f relativeVelocity = velB - velA;

        Vector2f tangent = relativeVelocity - relativeVelocity * contact.normal * contact.normal;
        if (Vector2f::ApproxEquals(tangent, Vector2f::Zero))
          continue;
        tangent.Normalize();

        // Compute the impulse
        const float raPerpMag = raPerp * tangent;
        const float rbPerpMag = rbPerp * tangent;
        const float rb1Denom = contact.rb1 ? contact.rb1->massInv + raPerpMag * raPerpMag * contact.rb1->inertiaInv : 0.0f;
        const float rb2Denom = contact.rb2 ? contact.rb2->massInv + rbPerpMag * rbPerpMag * contact.rb2->inertiaInv : 0.0f;
        float denominator = rb1Denom + rb2Denom;
        if (denominator <= 0.0f)
          denominator = 1.0f;
        if (const float jt = -relativeVelocity * tangent / (denominator * contact.contactPoints.size());
          std::abs(jt) <= jList[i] * sf)
          frictionImpulses[i] = jt * tangent;
        else
          frictionImpulses[i] = -jt * tangent * df;
      }

      // Apply the friction impulses to update the linear and angular velocity
      for (int i = 0; i < frictionImpulses.size(); i++) {
        if (rb1NotStaticOrKinematic) {
          const Vector2f ra = contact.contactPoints[i] - contact.col1->Transform()->WorldPosition();
          contact.rb1->linearVelocity -= frictionImpulses[i] * contact.rb1->massInv;
          contact.rb1->angularVelocity -= frictionImpulses[i] ^ ra * contact.rb1->inertiaInv;
        }
        if (rb2NotStaticOrKinematic) {
          const Vector2f rb = contact.contactPoints[i] - contact.col2->Transform()->WorldPosition();
          contact.rb2->linearVelocity += frictionImpulses[i] * contact.rb2->massInv;
          contact.rb2->angularVelocity += frictionImpulses[i] ^ rb * contact.rb2->inertiaInv;
        }
      }
    }
  }
}
