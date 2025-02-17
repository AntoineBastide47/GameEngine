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
}

using namespace Engine2D::Physics;

namespace Engine::Physics {
  class CollisionManifold {
    public:
      /// The first collider involved in the collision
      std::shared_ptr<Collider2D> col1;
      /// The second collider involved in the collision
      std::shared_ptr<Collider2D> col2;
      /// The first rigidbody involved in the collision
      std::shared_ptr<Rigidbody2D> rb1;
      /// The second rigidbody involved in the collision
      std::shared_ptr<Rigidbody2D> rb2;
      /// The normal along which the collision occurs
      glm::vec2 normal;
      /// The first contact point between the two rigidbodies
      std::vector<glm::vec2> contactPoints;

      CollisionManifold(
        const std::shared_ptr<Collider2D> &col1, const std::shared_ptr<Collider2D> &col2,
        const std::shared_ptr<Rigidbody2D> &rb1, const std::shared_ptr<Rigidbody2D> &rb2, glm::vec2 normal,
        glm::vec2 contactPoint1, glm::vec2 contactPoint2, uint8_t contactCount
      );
  };
}

#endif //COLLISION_MANIFOLD_H
