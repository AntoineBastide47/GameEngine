//
// CollisionManifold.cpp
// Author: Antoine Bastide
// Date: 08/12/2024
//

#include "2D/Physics/CollisionManifold.h"

namespace Engine::Physics {
  CollisionManifold::CollisionManifold(
    const std::shared_ptr<Rigidbody2D> &rb1, const std::shared_ptr<Rigidbody2D> &rb2, const Engine2D::Vector2f normal,
    const double depth, const Engine2D::Vector2f contactPoint1, const Engine2D::Vector2f contactPoint2,
    const uint8_t contactCount
  )
    : rb1(rb1), rb2(rb2), normal(normal), depth(depth), contactPoints({contactPoint1}) {
    if (contactCount == 2)
      contactPoints.emplace_back(contactPoint2);
  }
}
