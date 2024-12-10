//
// Physics2D.cpp
// Author: Antoine Bastide
// Date: 29/11/2024
//

#include <ranges>

#include "2D/Physics/Physics2D.h"
#include "2D/Entity2D.h"
#include "2D/Physics/Collisions.h"
#include "2D/Physics/Rigidbody2D.h"
#include "Common/Log.h"

namespace Engine2D::Physics {
  const Vector2 Physics2D::gravity{0.0f, -9.81f};

  Physics2D::Physics2D(const float fixedDeltaTime) : fixedDeltaTime(fixedDeltaTime) {}

  void Physics2D::step() {
    if (rigidbodies.empty())
      return;

    bool foundNull = false;
    for (const auto &rigidbody: rigidbodies) {
      if (!rigidbody)
        foundNull = true;
      else if (!rigidbody->isStatic)
        rigidbody->Step(fixedDeltaTime);
    }

    if (foundNull)
      std::erase_if(
        rigidbodies, [](const Rigidbody2D *ptr) {
          return ptr == nullptr;
        }
      );

    for (auto it1 = rigidbodies.begin(); it1 != rigidbodies.end() - 1; ++it1) {
      Rigidbody2D * &rb1 = *it1;
      // Do nothing if the collider is null or not active
      if (!rb1->Entity() || !rb1->Entity()->IsActive())
        continue;
      for (auto it2 = it1 + 1; it2 != rigidbodies.end(); ++it2) {
        Rigidbody2D * &rb2 = *it2;
        // Do nothing if the collider is null or not active
        if (!rb2->Entity() || !rb2->Entity()->IsActive())
          continue;
        // Do nothing if both colliders are static
        if (rb1->isStatic && rb2->isStatic)
          continue;

        Vector2 normal;
        float depth = std::numeric_limits<float>::max();
        if (Collisions::collide(rb1, rb2, &normal, &depth)) {
          Vector2 move = normal * depth * 0.5;

          if (rb1->isStatic)
            rb2->Transform()->position -= move * 2;
          else if (rb2->isStatic)
            rb1->Transform()->position += move * 2;
          else {
            rb1->Transform()->position -= move;
            rb2->Transform()->position += move;
          }

          contactList.emplace_back(rb1, rb2, normal, depth, Vector2::Zero, Vector2::Zero, 0);
        }
      }
    }

    for (auto &contact: contactList) {
      ResolveCollision(contact);
      contact.rb1->Entity()->OnCollision(contact.rb2);
      contact.rb2->Entity()->OnCollision(contact.rb1);
    }
    contactList.clear();

    if (!rigidbodiesToRemove.empty()) {
      for (const Rigidbody2D *rigidbody: rigidbodiesToRemove)
        if (auto it = std::ranges::find(rigidbodies, rigidbody); it != rigidbodies.end())
          rigidbodies.erase(it);
      rigidbodiesToRemove.clear();
    }
  }

  void Physics2D::ResolveCollision(const Engine::Physics::CollisionManifold &contact) {
    if (const float d = (contact.rb2->linearVelocity - contact.rb1->linearVelocity) * contact.normal; d < 0.0f) {
      const float e = std::min(contact.rb1->restitution, contact.rb2->restitution);
      const float j = -(1 + e) * d / (contact.rb1->massInv + contact.rb2->massInv);
      Engine::Log::println((j * contact.normal).ToString());
      if (!contact.rb1->isStatic)
        contact.rb1->linearVelocity -= j * contact.normal * contact.rb1->massInv;
      if (!contact.rb2->isStatic)
        contact.rb2->linearVelocity += j * contact.normal * contact.rb2->massInv;
    }
  }

  void Physics2D::addRigidBody(Rigidbody2D *rigidbody) {
    if (rigidbody)
      rigidbodies.push_back(rigidbody);
  }

  void Physics2D::removeRigidbody(const Rigidbody2D *rigidbody) {
    if (rigidbody)
      rigidbodiesToRemove.insert(rigidbody);
  }
}
