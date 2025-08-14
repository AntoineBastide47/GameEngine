//
// Physics2D.cpp
// Author: Antoine Bastide
// Date: 29/11/2024
//

#include <algorithm>
#include <future>
#include <numeric>
#include <unordered_set>
#include <vector>

#include "Engine2D/Physics/Physics2D.hpp"
#include "Engine/Settings.hpp"
#include "Engine/Macros/Profiling.hpp"
#include "Engine2D/Behaviour.hpp"
#include "Engine2D/Entity2D.hpp"
#include "Engine2D/Physics/CollisionGrid.hpp"
#include "Engine2D/Physics/CollisionManifold.hpp"
#include "Engine2D/Physics/Collisions.hpp"
#include "Engine2D/Physics/Rigidbody2D.hpp"
#include "Engine2D/Types/Vector2.hpp"

namespace Engine2D::Physics {
  Physics2D::Physics2D()
    : initialized(false), collisionGridNeedsResizing(false) {}

  Physics2D::~Physics2D() {
    colliders.clear();
    collidersToAdd.clear();
    collidersToRemove.clear();
    activeColliders.clear();
    contactPairs.clear();
    previousCollisionPairs.clear();
  }

  void Physics2D::addCollider(Collider2D *collider) {
    if (!collider)
      return;
    colliders.emplace_back(collider);
  }

  void Physics2D::removeCollider(Collider2D *collider) {
    if (!collider)
      return;
    std::erase(colliders, collider);
  }

  void Physics2D::step() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    if (!initialized) {
      collisionGrid = CollisionGrid{Engine::Settings::Physics::GetPartitionSize()};
      initialized = true;
    }

    // Skip the collision detection if there are no active colliders
    findActiveColliders();
    if (!activeColliders.empty()) {
      for (const auto collider: activeColliders)
        if (const auto rb = collider->rigidbody; rb && rb->IsActive())
          rb->step();

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
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSubSystem);
    for (const auto collider: colliders)
      if (collider->IsActive())
        activeColliders.push_back(collider);
  }

  void Physics2D::notifyCollisions(
    const Collider2D *sender, const Collider2D *receiver,
    const CollisionEventType eventType
  ) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

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

  void Physics2D::broadPhase(const std::vector<Collider2D *> &collidersToChecks) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    // Precompute AABBs
    std::vector<std::pair<Collider2D *, Collider2D::AABB>> colliderPairs;
    colliderPairs.reserve(collidersToChecks.size());
    for (const auto col: collidersToChecks)
      colliderPairs.push_back({col, col->getAABB()});

    // Sweep and Prune: Sort colliders by AABB min.x
    std::ranges::sort(
      colliderPairs, [](const auto &a, const auto &b) {
        return a.second.min.x < b.second.min.x;
      }
    );

    // Collision check loop
    for (size_t i = 0; i < colliderPairs.size(); ++i) {
      const auto &[col1, col1AABB] = colliderPairs[i];

      for (size_t j = i + 1; j < colliderPairs.size(); ++j) {
        const auto &[col2, col2AABB] = colliderPairs[j];

        // Stop if no overlap on X-axis
        if (col2AABB.min.x > col1AABB.max.x)
          break;

        // AABB overlap check on Y-axis and self-collisions
        if (col2AABB.min.y > col1AABB.max.y || col1AABB.min.y > col2AABB.max.y)
          continue;

        // Skip self-collisions
        if (col1->Entity() == col2->Entity())
          continue;

        // Parent-child relationship check, making sure that both have a rigidbody
        const auto rb1 = col1->rigidbody;
        const auto rb2 = col2->rigidbody;
        const bool col1ChildCol2 = col1->Entity()->Transform()->IsChildOf(col2->Entity()) && !(rb1 && rb2);
        const bool col2ChildCol1 = col2->Entity()->Transform()->IsChildOf(col1->Entity()) && !(rb1 && rb2);
        if ((!rb1 && !rb2) || col1ChildCol2 || col2ChildCol1)
          continue;

        // Canonical ordering for each contact pair
        ContactPair contactPair{
          col1 < col2 ? col1 : col2,
          col1 < col2 ? col2 : col1,
          col1 < col2 ? rb1 : rb2,
          col1 < col2 ? rb2 : rb1
        };

        // Handle previous collision exit notifications
        if (previousCollisionPairs.contains(contactPair)) {
          notifyCollisions(col1, col2, Exit);
          notifyCollisions(col2, col1, Exit);
        }

        // Store contact pair
        contactPairs.push_back(contactPair);
      }
    }
  }

  void Physics2D::narrowPhase() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    if (contactPairs.empty()) {
      previousCollisionPairs.clear();
      return;
    }

    std::unordered_set<ContactPair> collisionPairs;
    for (auto &[col1, col2, rb1, rb2]: contactPairs) {
      // More accurate SAT collision check
      glm::vec<2, double> normal;
      double depth = std::numeric_limits<double>::max();
      if (Collisions::collide(col1, col2, &normal, &depth)) {
        if (!col1->isTrigger && !col2->isTrigger)
          separateBodies(col1, col2, rb1, rb2, normal * depth);

        ContactPair collisionPair{col1, col2, rb1, rb2};
        const CollisionEventType collisionType = previousCollisionPairs.contains(collisionPair) ? Stay : Enter;

        // Find contact points
        glm::vec2 contactPoint1, contactPoint2;
        uint8_t contactCount = 0;
        Collisions::findContactPoints(col1, col2, &contactPoint1, &contactPoint2, &contactCount);

        // Resolve the collision by inverting the normal to make it point outwards
        CollisionManifold contact{
          col1, col2, rb1, rb2, -normal, contactPoint1, contactPoint2, contactCount
        };
        col1->contactPoints = contact.contactPoints;
        col2->contactPoints = contact.contactPoints;

        if (collisionType == Enter && !col1->isTrigger && !col2->isTrigger)
          resolveCollision(contact);

        notifyCollisions(col1, col2, collisionType);
        notifyCollisions(col2, col1, collisionType);

        collisionPairs.insert(collisionPair);
      }
    }
    // Delete the previous collision pairs and replace them by the new ones
    previousCollisionPairs = std::move(collisionPairs);
  }

  void Physics2D::separateBodies(
    const Collider2D *col1, const Collider2D *col2, const Rigidbody2D *rb1, const Rigidbody2D *rb2, const glm::vec2 mtv
  ) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

    const bool separateBody1 = rb1 && !rb1->isKinematic && !col1->isTrigger;
    const bool separateBody2 = rb2 && !rb2->isKinematic && !col2->isTrigger;
    const float invMass1 = rb1 && separateBody1 ? rb1->massInv : 0.0f;
    const float invMass2 = rb2 && separateBody2 ? rb2->massInv : 0.0f;
    const float ratio1 = invMass1 / (invMass1 + invMass2);
    const float ratio2 = invMass2 / (invMass1 + invMass2);

    if (separateBody1 && !separateBody2)
      col1->Transform()->UpdatePosition(ratio1 * mtv);
    else if (!separateBody1 && separateBody2)
      col2->Transform()->UpdatePosition(-ratio2 * mtv);
    else {
      col1->Transform()->UpdatePosition(ratio1 * mtv);
      col2->Transform()->UpdatePosition(-ratio2 * mtv);
    }
  }

  void Physics2D::resolveCollision(const CollisionManifold &contact) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSubSystem);

    float elasticity = std::max(contact.col1->elasticity, contact.col2->elasticity);
    if (elasticity > 1.0f)
      elasticity = 1.0f;

    std::array<glm::vec2, 2> ra{}, rb{};
    std::array<glm::vec2, 2> raPerp{}, rbPerp{};
    std::array<float, 2> normalAccum{};
    std::array<float, 2> tangentAccum{};
    std::array<float, 2> invMassSum{};
    constexpr size_t maxIterations = 10;
    const auto N = contact.contactPoints.size();

    if (contact.rb1 && !contact.rb1->isKinematic)
      contact.rb1->computeInertia(contact.col1);
    if (contact.rb2 && !contact.rb2->isKinematic)
      contact.rb2->computeInertia(contact.col2);

    // Find the impulses to apply
    for (int i = 0; i < N; i++) {
      ra[i] = contact.contactPoints[i] - contact.col1->Transform()->GetWorldPosition();
      rb[i] = contact.contactPoints[i] - contact.col2->Transform()->GetWorldPosition();
      raPerp[i] = glm::perpendicular(ra[i]);
      rbPerp[i] = glm::perpendicular(rb[i]);
      const float raPerpMag = glm::dot(raPerp[i], contact.normal);
      const float rbPerpMag = glm::dot(rbPerp[i], contact.normal);
      const float rb1Denom = contact.rb1 && !contact.rb1->isKinematic
                               ? contact.rb1->massInv + raPerpMag * raPerpMag * contact.rb1->inertiaInv
                               : 0.0f;
      const float rb2Denom = contact.rb2 && !contact.rb2->isKinematic
                               ? contact.rb2->massInv + rbPerpMag * rbPerpMag * contact.rb2->inertiaInv
                               : 0.0f;
      invMassSum[i] = std::max(rb1Denom + rb2Denom, 1e-6f);
    }

    for (size_t it = 0; it < maxIterations; it++) {
      bool done = true;
      for (int i = 0; i < N; i++) {
        // Find the relative velocity
        const auto velA = contact.rb1
                            ? raPerp[i] * contact.rb1->angularVelocity + contact.rb1->linearVelocity
                            : glm::vec2(0);
        const auto velB = contact.rb2
                            ? rbPerp[i] * contact.rb2->angularVelocity + contact.rb2->linearVelocity
                            : glm::vec2(0);
        const auto relativeVelocity = velB - velA;
        const float vn = glm::dot(relativeVelocity, contact.normal);
        if (vn > -0.01f)
          continue;

        // Below 1 m/s bouncing is imperceptible
        float e = elasticity;
        if (std::fabs(vn) < 0.2f && e < 0.8f)
          e = 0.0f;
        float j = -(1 + e) * vn;
        j /= invMassSum[i];

        // Clamp the accumulated normal impulse (total must be >= 0)
        float oldImpulse = normalAccum[i];
        float newImpulse = std::max(oldImpulse + j, 0.0f);
        float deltaJ = newImpulse - oldImpulse;
        normalAccum[i] = newImpulse;

        if (std::fabs(deltaJ) > 1e-5f)
          done = false;

        // Apply incremental impulse
        glm::vec2 P = deltaJ * contact.normal;
        if (contact.rb1 && !contact.rb1->isKinematic) {
          contact.rb1->linearVelocity -= P * contact.rb1->massInv;
          contact.rb1->angularVelocity += glm::cross_2(P, ra[i]) * contact.rb1->inertiaInv;
        }
        if (contact.rb2 && !contact.rb2->isKinematic) {
          contact.rb2->linearVelocity += P * contact.rb2->massInv;
          contact.rb2->angularVelocity -= glm::cross_2(P, rb[i]) * contact.rb2->inertiaInv;
        }
      }

      float sf = 0;
      float df = 0;
      if (contact.rb1 && contact.rb2) {
        sf = std::sqrt(contact.rb1->staticFriction * contact.rb2->staticFriction);
        df = std::sqrt(contact.rb1->dynamicFriction * contact.rb2->dynamicFriction);
      } else if (contact.rb1) {
        sf = contact.rb1->staticFriction;
        df = contact.rb1->dynamicFriction;
      } else if (contact.rb2) {
        sf = contact.rb2->staticFriction;
        df = contact.rb2->dynamicFriction;
      }

      for (int i = 0; i < N; i++) {
        // recompute post–bounce relative velocity
        const auto velA = contact.rb1
                            ? raPerp[i] * contact.rb1->angularVelocity + contact.rb1->linearVelocity
                            : glm::vec2(0.0f);
        const auto velB = contact.rb2
                            ? rbPerp[i] * contact.rb2->angularVelocity + contact.rb2->linearVelocity
                            : glm::vec2(0.0f);
        const auto relativeVelocity = velB - velA;

        // compute tangent
        glm::vec2 tangent = relativeVelocity - glm::dot(relativeVelocity, contact.normal) * contact.normal;
        float speed = glm::length(tangent);
        if (speed < 1e-6f)
          continue;
        tangent /= speed;

        const float raPerpMag = glm::dot(raPerp[i], tangent);
        const float rbPerpMag = glm::dot(rbPerp[i], tangent);
        const float m1 = contact.rb1 ? contact.rb1->massInv + raPerpMag * raPerpMag * contact.rb1->inertiaInv : 0.0f;
        const float m2 = contact.rb2 ? contact.rb2->massInv + rbPerpMag * rbPerpMag * contact.rb2->inertiaInv : 0.0f;
        const float invMass = std::max(m1 + m2, 1e-6f);

        const float vt = glm::dot(relativeVelocity, tangent);
        const float jt = -vt / invMass;
        const float maxStatic = normalAccum[i] * sf;
        const float maxDynamic = normalAccum[i] * df;
        const float oldTangentImpulse = tangentAccum[i];
        float newTangentImpulse = oldTangentImpulse + jt;

        if (std::fabs(newTangentImpulse) > maxStatic)
          newTangentImpulse = std::clamp(newTangentImpulse, -maxDynamic, maxDynamic);
        float dJt = newTangentImpulse - oldTangentImpulse;
        tangentAccum[i] = newTangentImpulse;

        if (std::fabs(dJt) > 1e-5f)
          done = false;

        const auto frictionImpulse = dJt * tangent;
        if (contact.rb1 && !contact.rb1->isKinematic) {
          contact.rb1->linearVelocity -= frictionImpulse * contact.rb1->massInv;
          contact.rb1->angularVelocity += glm::cross_2(frictionImpulse, ra[i]) * contact.rb1->inertiaInv;
        }
        if (contact.rb2 && !contact.rb2->isKinematic) {
          contact.rb2->linearVelocity += frictionImpulse * contact.rb2->massInv;
          contact.rb2->angularVelocity -= glm::cross_2(frictionImpulse, rb[i]) * contact.rb2->inertiaInv;
        }
      }
      if (done)
        break;
    }
  }
}
