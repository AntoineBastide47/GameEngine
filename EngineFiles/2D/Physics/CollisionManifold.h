//
// CollisionManifold.h
// Author: Antoine Bastide
// Date: 08/12/2024
//

#ifndef COLLISION_MANIFOLD_H
#define COLLISION_MANIFOLD_H

#include <vector>

#include "2D/Types/Vector2.h"

namespace Engine2D::Physics {
  class Rigidbody2D;
}
namespace Engine::Physics {
  class CollisionManifold {
    public:
      /// The first rigidbody involved in the collision
      Engine2D::Physics::Rigidbody2D *rb1;
      /// The second rigidbody involved in the collision
      Engine2D::Physics::Rigidbody2D *rb2;
      /// The normal along which the collision occurs
      Engine2D::Vector2 normal;
      /// The depth of the collision
      float depth;
      /// The first contact point between the two rigidbodies
      std::vector<Engine2D::Vector2> contactPoints;

      CollisionManifold(
        Engine2D::Physics::Rigidbody2D *rb1, Engine2D::Physics::Rigidbody2D *rb2, Engine2D::Vector2 normal, float depth,
        Engine2D::Vector2 contactPoint1, Engine2D::Vector2 contactPoint2, uint8_t contactCount
      );
  };
}

#endif //COLLISION_MANIFOLD_H