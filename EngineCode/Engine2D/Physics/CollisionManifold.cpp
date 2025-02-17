//
// CollisionManifold.cpp
// Author: Antoine Bastide
// Date: 08/12/2024
//

#include "Engine2D/Physics/CollisionManifold.h"

namespace Engine::Physics {
  CollisionManifold::CollisionManifold(
    const std::shared_ptr<Collider2D> &col1, const std::shared_ptr<Collider2D> &col2,
    const std::shared_ptr<Rigidbody2D> &rb1, const std::shared_ptr<Rigidbody2D> &rb2, const glm::vec2 normal,
    const glm::vec2 contactPoint1, const glm::vec2 contactPoint2, const uint8_t contactCount
  )
    : col1(col1), col2(col2), rb1(rb1), rb2(rb2), normal(normal), contactPoints({contactPoint1}) {
    if (contactCount == 2)
      contactPoints.emplace_back(contactPoint2);
  }
}
