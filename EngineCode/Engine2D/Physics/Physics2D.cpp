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
#include "Engine/Settings.h"
#include "Engine2D/Behaviour.h"
#include "Engine2D/Types/Vector2.h"

namespace Engine2D::Physics {
  Physics2D::Physics2D() : collisionGridNeedsResizing(false), collisionGrid(Engine::Settings::Physics::GetPartitionSize()) {}

  Physics2D::~Physics2D() {
    colliders.clear();
    collidersToAdd.clear();
    collidersToRemove.clear();
    activeColliders.clear();
    contactPairs.clear();
    previousCollisionPairs.clear();
  }

  void Physics2D::addCollider(const std::shared_ptr<Collider2D> &collider) {
    if (collider)
      collidersToAdd.emplace_back(collider);
  }

  void Physics2D::addColliders() {
    if (!collidersToAdd.empty())
      for (auto collider: collidersToAdd)
        colliders.emplace_back(collider);
    collidersToAdd.clear();
  }

  void Physics2D::removeCollider(const std::shared_ptr<Collider2D> &collider) {
    if (collider)
      collidersToRemove.emplace_back(collider);
  }

  void Physics2D::removeColliders() {
    if (!collidersToRemove.empty())
      for (auto collider: collidersToRemove)
        std::erase(colliders, collider);
    collidersToRemove.clear();
  }

  void Physics2D::step() {
    // Update the collider vector
    addColliders();
    removeColliders();

    // Skip the collision detection if there are no active colliders
    findActiveColliders();
    if (!activeColliders.empty()) {
      for (const auto collider: activeColliders)
        if (collider->rigidbody)
          collider->rigidbody->step();

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
        case Stay:
          for (const auto &behaviour: sender->Entity()->behaviours)
            behaviour->OnTriggerStay2D(receiver);
          break;
        case Enter:
          for (const auto &behaviour: sender->Entity()->behaviours)
            behaviour->OnTriggerEnter2D(receiver);
          break;
        case Exit:
          for (const auto &behaviour: sender->Entity()->behaviours)
            behaviour->OnTriggerExit2D(receiver);
          break;
      }
    } else {
      switch (eventType) {
        case Stay:
          for (const auto &behaviour: sender->Entity()->behaviours)
            behaviour->OnCollisionStay2D(receiver);
          break;
        case Enter:
          for (const auto &behaviour: sender->Entity()->behaviours)
            behaviour->OnCollisionEnter2D(receiver);
          break;
        case Exit:
          for (const auto &behaviour: sender->Entity()->behaviours)
            behaviour->OnCollisionExit2D(receiver);
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
        // Check if the colliders have a parent-child relationship and if they do not have a rigidbody each
        const bool col1ChildCol2 = col1->Entity()->transform->IsChildOf(col2->Entity()) && !(rb1 && rb2);
        const bool col2ChildCol1 = col2->Entity()->transform->IsChildOf(col1->Entity()) && !(rb1 && rb2);
        // Make sure it is not a collision between two static colliders, prevent entities colliding with themselves
        // and do a cheap collision check for early out
        if ((!rb1 && !rb2) || col1->Entity() == col2->Entity() || col1ChildCol2 || col2ChildCol1 ||
            !Collisions::collideAABB(col1AABB, col2->getAABB())) {
          if (std::ranges::find(previousCollisionPairs, contactPair) != previousCollisionPairs.end()) {
            notifyCollisions(col1, col2, Exit);
            notifyCollisions(col2, col1, Exit);
          }

          continue;
        }

        // Save the indices of the bodies that collided using Canonical ordering
        contactPairs.emplace_back(contactPair);
      }
    }
  }

  void Physics2D::narrowPhase() {
    if (contactPairs.empty()) {
      previousCollisionPairs.clear();
      return;
    }

    std::vector<ContactPair> collisionPairs;
    for (auto &[col1, col2, rb1, rb2]: contactPairs) {
      // More accurate SAT collision check
      glm::vec<2, double> normal;
      double depth = std::numeric_limits<double>::max();
      if (Collisions::collide(col1, col2, &normal, &depth)) {
        separateBodies(col1, col2, rb1, rb2, normal * depth);

        // Notify the entities of the collisions
        ContactPair collisionPair{col1, col2, rb1, rb2};
        const CollisionEventType collisionType =
          std::ranges::find(previousCollisionPairs, collisionPair) != previousCollisionPairs.end() ? Stay : Enter;

        // Find contact points
        glm::vec2 contactPoint1, contactPoint2;
        uint8_t contactCount = 0;
        Collisions::findContactPoints(col1, col2, &contactPoint1, &contactPoint2, &contactCount);

        // Resolve the collision by inverting the normal to make it point outwards
        Engine::Physics::CollisionManifold contact{
          col1, col2, rb1, rb2, -normal, contactPoint1, contactPoint2, contactCount
        };
        col1->contactPoints = contact.contactPoints;
        col2->contactPoints = contact.contactPoints;

        // Make sure to only apply a single impulse
        if (collisionType == Enter)
          resolveCollision(contact);

        notifyCollisions(col1, col2, collisionType);
        notifyCollisions(col2, col1, collisionType);

        collisionPairs.emplace_back(collisionPair);
      }

      // Delete the previous collision pairs and replace them by the new ones
      previousCollisionPairs = std::move(collisionPairs);
    }
  }

  void Physics2D::separateBodies(
    const std::shared_ptr<Collider2D> &col1, const std::shared_ptr<Collider2D> &col2,
    const std::shared_ptr<Rigidbody2D> &rb1, const std::shared_ptr<Rigidbody2D> &rb2, const glm::vec2 mtv
  ) {
    const bool separateBody1 = rb1 && !rb1->isKinematic && !col1->isTrigger;
    const bool separateBody2 = rb2 && !rb2->isKinematic && !col2->isTrigger;
    const float invMass1 = separateBody1 ? rb1->massInv : 0.0f;
    const float invMass2 = separateBody2 ? rb2->massInv : 0.0f;
    const float ratio1 = invMass1 / (invMass1 + invMass2);
    const float ratio2 = invMass2 / (invMass1 + invMass2);

    if (separateBody1 && !separateBody2)
      col1->Transform()->UpdatePosition(ratio1 * mtv);
    else if (!separateBody1 && separateBody2)
      col2->Transform()->UpdatePosition(-ratio2 * mtv);
    else {
      col1->Transform()->UpdatePosition(+ratio1 * mtv * 0.5f);
      col2->Transform()->UpdatePosition(-ratio2 * mtv * 0.5f);
    }
  }

  void Physics2D::resolveCollision(const Engine::Physics::CollisionManifold &contact) {
    const float bounciness = std::min(contact.col1->bounciness, contact.col2->bounciness);
    std::array<glm::vec2, 2> impulses;
    std::array<float, 2> jList;
    const bool rb1NotStaticOrKinematic = !(!contact.rb1 || (contact.rb1 && contact.rb1->isKinematic));
    const bool rb2NotStaticOrKinematic = !(!contact.rb2 || (contact.rb2 && contact.rb2->isKinematic));

    if (rb1NotStaticOrKinematic)
      contact.rb1->computeInertia(contact.col1);
    if (rb2NotStaticOrKinematic)
      contact.rb2->computeInertia(contact.col2);

    // Find the impulses to apply
    for (int i = 0; i < contact.contactPoints.size(); i++) {
      const glm::vec2 raPerp = glm::perpendicular(contact.contactPoints[i] - contact.col1->Transform()->GetWorldPosition());
      const glm::vec2 rbPerp = glm::perpendicular(contact.contactPoints[i] - contact.col2->Transform()->GetWorldPosition());

      // Find the relative velocity
      const auto velA = contact.rb1 ? raPerp * contact.rb1->angularVelocity + contact.rb1->linearVelocity : glm::vec2(0);
      const auto velB = contact.rb2 ? rbPerp * contact.rb2->angularVelocity + contact.rb2->linearVelocity : glm::vec2(0);
      const glm::vec2 relativeVelocity = velB - velA;

      if (const float contactVelocityMag = glm::dot(relativeVelocity, contact.normal); contactVelocityMag <= 0.0f) {
        // Compute the impulse
        const float raPerpMag = glm::dot(raPerp, contact.normal);
        const float rbPerpMag = glm::dot(rbPerp, contact.normal);
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
    for (int i = 0; i < contact.contactPoints.size(); i++) {
      if (rb1NotStaticOrKinematic) {
        const glm::vec2 ra = contact.contactPoints[i] - contact.col1->Transform()->GetWorldPosition();
        contact.rb1->linearVelocity -= impulses[i] * contact.rb1->massInv;
        contact.rb1->angularVelocity += glm::cross_2(impulses[i], ra) * contact.rb1->inertiaInv;
      }
      if (rb2NotStaticOrKinematic) {
        const glm::vec2 rb = contact.contactPoints[i] - contact.col2->Transform()->GetWorldPosition();
        contact.rb2->linearVelocity += impulses[i] * contact.rb2->massInv;
        contact.rb2->angularVelocity -= glm::cross_2(impulses[i], rb) * contact.rb2->inertiaInv;
      }
    }

    const bool frictionEnabled = Engine::Settings::Physics::GetFrictionEnabled();
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
    std::array<glm::vec2, 2> frictionImpulses;

    // Find the friction impulses to apply
    for (int i = 0; i < contact.contactPoints.size(); i++) {
      const glm::vec2 raPerp = glm::perpendicular(contact.contactPoints[i] - contact.col1->Transform()->GetWorldPosition());
      const glm::vec2 rbPerp = glm::perpendicular(contact.contactPoints[i] - contact.col2->Transform()->GetWorldPosition());

      // Find the relative velocity
      const auto velA = rb1NotStaticOrKinematic
                          ? raPerp * contact.rb1->angularVelocity + contact.rb1->linearVelocity
                          : glm::vec2(0);
      const auto velB = rb2NotStaticOrKinematic
                          ? rbPerp * contact.rb2->angularVelocity + contact.rb2->linearVelocity
                          : glm::vec2(0);
      const glm::vec2 relativeVelocity = velB - velA;

      glm::vec2 tangent = relativeVelocity - relativeVelocity * contact.normal * contact.normal;
      if (glm::approx_equals(tangent, glm::vec2(0)))
        continue;
      tangent = glm::normalize(tangent);

      // Compute the impulse
      const float raPerpMag = glm::dot(raPerp, tangent);
      const float rbPerpMag = glm::dot(rbPerp, tangent);
      const float rb1Denom = contact.rb1 ? contact.rb1->massInv + raPerpMag * raPerpMag * contact.rb1->inertiaInv : 0.0f;
      const float rb2Denom = contact.rb2 ? contact.rb2->massInv + rbPerpMag * rbPerpMag * contact.rb2->inertiaInv : 0.0f;
      float denominator = rb1Denom + rb2Denom;
      if (denominator <= 0.0f)
        denominator = 1.0f;
      if (const float jt = glm::dot(-relativeVelocity, tangent) / (denominator * contact.contactPoints.size());
        std::abs(jt) <= jList[i] * sf)
        frictionImpulses[i] = jt * tangent;
      else
        frictionImpulses[i] = -jt * tangent * df;
    }

    // Apply the friction impulses to update the linear and angular velocity
    for (int i = 0; i < contact.contactPoints.size(); i++) {
      if (rb1NotStaticOrKinematic) {
        const glm::vec2 ra = contact.contactPoints[i] - contact.col1->Transform()->GetWorldPosition();
        if (frictionEnabled)
          contact.rb1->linearVelocity -= frictionImpulses[i] * contact.rb1->massInv;
        contact.rb1->angularVelocity += glm::cross_2(frictionImpulses[i], ra) * contact.rb1->inertiaInv;
      }
      if (rb2NotStaticOrKinematic) {
        const glm::vec2 rb = contact.contactPoints[i] - contact.col2->Transform()->GetWorldPosition();
        if (frictionEnabled)
          contact.rb2->linearVelocity += frictionImpulses[i] * contact.rb2->massInv;
        contact.rb2->angularVelocity -= glm::cross_2(frictionImpulses[i], rb) * contact.rb2->inertiaInv;
      }
    }
  }
}
