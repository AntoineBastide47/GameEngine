//
// CollisionManifold.cpp
// Author: Antoine Bastide
// Date: 08/12/2024
//

#include "2D/Physics/CollisionManifold.h"

namespace Engine::Physics {
  CollisionManifold::CollisionManifold(
    Engine2D::Physics::Rigidbody2D *rb1, Engine2D::Physics::Rigidbody2D *rb2, const Engine2D::Vector2 normal, const float depth,
    const Engine2D::Vector2 contactPoint1, const Engine2D::Vector2 contactPoint2, const uint8_t contactCount
  )
    : rb1(rb1), rb2(rb2), normal(normal), depth(depth), contactPoints({contactPoint1}) {
    if (contactCount == 2)
      contactPoints.emplace_back(contactPoint2);
  }
}
