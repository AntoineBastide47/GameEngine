//
// CollisionManifold.h
// Author: Antoine Bastide
// Date: 08/12/2024
//

#ifndef COLLISION_MANIFOLD_H
#define COLLISION_MANIFOLD_H
#include "2D/Types/Vector2.h"

namespace Engine2D::Physics {
  class Rigidbody2D;
}
namespace Engine::Physics {
  class CollisionManifold {
    public:
      Engine2D::Physics::Rigidbody2D *rb1;
      Engine2D::Physics::Rigidbody2D *rb2;
      Engine2D::Vector2 normal;
      float depth;
      Engine2D::Vector2 contactPoint1;
      Engine2D::Vector2 contactPoint2;
      uint8_t contactCount;

      CollisionManifold(
        Engine2D::Physics::Rigidbody2D *rb1, Engine2D::Physics::Rigidbody2D *rb2, Engine2D::Vector2 normal, float depth,
        Engine2D::Vector2 contactPoint1, Engine2D::Vector2 contactPoint2, uint8_t contactCount
      );
  };
}

#endif //COLLISION_MANIFOLD_H
