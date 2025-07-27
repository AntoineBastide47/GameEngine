//
// CollisionManifold.h
// Author: Antoine Bastide
// Date: 08/12/2024
//

#ifndef COLLISION_MANIFOLD_H
#define COLLISION_MANIFOLD_H

#include <vector>
#include <glm/glm.hpp>

namespace Engine2D::Physics {
  class Collider2D;
  class Rigidbody2D;

  struct CollisionManifold {
    /// The first collider involved in the collision
    Collider2D *col1;
    /// The second collider involved in the collision
    Collider2D *col2;
    /// The first rigidbody involved in the collision
    Rigidbody2D *rb1;
    /// The second rigidbody involved in the collision
    Rigidbody2D *rb2;
    /// The normal along which the collision occurs
    glm::vec2 normal;
    /// The first contact point between the two rigidbodies
    std::vector<glm::vec2> contactPoints;

    CollisionManifold(
      Collider2D *col1, Collider2D *col2, Rigidbody2D *rb1, Rigidbody2D *rb2, glm::vec2 normal,
      glm::vec2 contactPoint1, glm::vec2 contactPoint2, uint8_t contactCount
    );
  };
}

#endif //COLLISION_MANIFOLD_H
