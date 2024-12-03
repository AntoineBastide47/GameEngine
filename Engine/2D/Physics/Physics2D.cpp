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
  Physics2D::Physics2D(const float fixedDeltaTime) : fixedDeltaTime(fixedDeltaTime), depth() {}

  void Physics2D::simulate() {
    std::erase_if(
      rigidbodies, [](const Rigidbody2D *ptr) {
        return ptr == nullptr;
      }
    );

    if (rigidbodies.empty())
      return;

    for (int i = 0; i < rigidbodies.size() - 1; ++i) {
      const auto rb1 = rigidbodies[i];
      // Do nothing if the collider is null or not active
      if (!rb1 || !rb1->Entity() || !rb1->Entity()->IsActive())
        continue;
      for (int j = i + 1; j < rigidbodies.size(); ++j) {
        const auto rb2 = rigidbodies[j];
        // Do nothing if the collider is null or not active
        if (!rb2 || !rb2->Entity() || !rb2->Entity()->IsActive())
          continue;
        // Do nothing if both colliders are static
        if (rb1->isStatic && rb2->isStatic)
          continue;

        if (collide(rb1, rb2, &normal, &depth)) {
          const Vector2 move = normal * depth * 0.5f;

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
    /*if (colliderA->Type() == Rigidbody2D::Rigidbody2DType::Rectangle) {
      if (colliderB->Type() == Rigidbody2D::Rigidbody2DType::Rectangle) {
        return Collisions::RectanglesIntersect(colliderA->Transform(), colliderB->Transform(), normal, depth);
      }
      if (colliderB->Type() == Rigidbody2D::Rigidbody2DType::Circle) {
        return Collisions::PolygonAndCircleIntersect(TODO, TODO, colliderB->Transform(), normal, depth);
      }
    }
    else if (colliderA->Type() == Rigidbody2D::Rigidbody2DType::Circle) {
      if (colliderB->Type() == Rigidbody2D::Rigidbody2DType::Rectangle) {
        const bool collide = Collisions::PolygonAndCircleIntersect(
          TODO, TODO, colliderA->Transform(), normal, depth
        );
        *normal = -*normal;
        return collide;
      }
      if (colliderB->Type() == Rigidbody2D::Rigidbody2DType::Circle) {
        return Collisions::CirclesIntersect(colliderA->Transform(), colliderB->Transform(), normal, depth);
      }
    }*/
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
