//
// CollisionManifold.cpp
// Author: Antoine Bastide
// Date: 08/12/2024
//

#include "2D/Physics/CollisionManifold.h"

namespace Engine::Physics {
  CollisionManifold::CollisionManifold(
    const std::shared_ptr<Collider2D> &col1, const std::shared_ptr<Collider2D> &col2,
    const std::shared_ptr<Rigidbody2D> &rb1, const std::shared_ptr<Rigidbody2D> &rb2, const Engine2D::Vector2f normal,
    Engine2D::Vector2f contactPoint1, Engine2D::Vector2f contactPoint2, const uint8_t contactCount
  )
    : col1(col1), col2(col2), rb1(rb1), rb2(rb2), normal(normal), contactPoints({contactPoint1}) {
    if (contactCount == 2)
      contactPoints.emplace_back(contactPoint2);
  }
}
