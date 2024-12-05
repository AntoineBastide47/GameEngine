//
// Physics2D.cpp
// Author: Antoine Bastide
// Date: 29/11/2024
//

#include "2D/Physics/Physics2D.h"
#include "2D/Entity2D.h"
#include "2D/Physics/Collisions.h"
#include "2D/Physics/Rigidbody2D.h"

namespace Engine2D::Physics {
  Physics2D::Physics2D(const float fixedDeltaTime) : fixedDeltaTime(fixedDeltaTime) {}

  void Physics2D::simulate() {
    if (rigidbodies.empty())
      return;

    bool foundNull = false;
    for (int i = 0; i < rigidbodies.size() - 1; ++i) {
      const auto rb1 = rigidbodies[i];
      // Do nothing if the collider is null or not active
      if (!rb1 || !rb1->Entity() || !rb1->Entity()->IsActive()) {
        foundNull = foundNull || !rb1;
        continue;
      }
      for (int j = i + 1; j < rigidbodies.size(); ++j) {
        const auto rb2 = rigidbodies[j];
        // Do nothing if the collider is null or not active
        if (!rb2 || !rb2->Entity() || !rb2->Entity()->IsActive()) {
          foundNull = foundNull || !rb2;
          continue;
        }
        // Do nothing if both colliders are static
        if (rb1->isStatic && rb2->isStatic)
          continue;

        Vector2 normal;
        float depth;
        if (collide(rb1, rb2, &normal, &depth)) {
          Vector2 move = normal * depth * 0.5f;

          if (rb1->isStatic)
            rb2->Transform()->position += move * 2;
          else if (rb2->isStatic)
            rb1->Transform()->position -= move * 2;
          else {
            rb1->Transform()->position -= move;
            rb2->Transform()->position += move;
          }

          rb1->Entity()->OnCollision(rb2);
          rb2->Entity()->OnCollision(rb1);
        }
      }
    }

    if (foundNull)
      std::erase_if(
        rigidbodies, [](const Rigidbody2D *ptr) {
          return ptr == nullptr;
        }
      );
  }

  void Physics2D::addRigidBody(Rigidbody2D *rigidbody) {
    if (!rigidbody)
      return;
    rigidbodies.push_back(rigidbody);
  }

  void Physics2D::removeRigidbody(const Rigidbody2D *rigidbody) {
    if (!rigidbody)
      return;
    if (const auto it = std::ranges::find(rigidbodies, rigidbody); it != rigidbodies.end())
      rigidbodies.erase(it);
  }

  bool Physics2D::collide(const Rigidbody2D *rigidbodyA, const Rigidbody2D *rigidbodyB, Vector2 *normal, float *depth) {
    if (rigidbodyA->Type() == Rigidbody2D::Circle && rigidbodyB->Type() == Rigidbody2D::Circle)
      return Collisions::CirclesIntersect(
        rigidbodyA->Transform()->position, rigidbodyA->Transform()->scale, rigidbodyB->Transform()->position,
        rigidbodyB->Transform()->scale, normal, depth
      );
    if (rigidbodyA->Type() == Rigidbody2D::Circle && rigidbodyB->Type() != Rigidbody2D::Circle) {
      const bool collide = Collisions::PolygonAndCircleIntersect(
        rigidbodyB->Vertices(), rigidbodyB->Transform()->position, rigidbodyA->Transform()->position,
        rigidbodyA->Transform()->scale, normal, depth
      );
      *normal = -*normal;
      return collide;
    }
    if (rigidbodyA->Type() != Rigidbody2D::Circle && rigidbodyB->Type() == Rigidbody2D::Circle) {
      return Collisions::PolygonAndCircleIntersect(
        rigidbodyA->Vertices(), rigidbodyA->Transform()->position, rigidbodyB->Transform()->position,
        rigidbodyB->Transform()->scale, normal, depth
      );
    }
    return Collisions::PolygonsIntersect(
      rigidbodyA->Vertices(), rigidbodyA->Transform()->position, rigidbodyB->Vertices(), rigidbodyB->Transform()->position,
      normal, depth
    );
  }
}
