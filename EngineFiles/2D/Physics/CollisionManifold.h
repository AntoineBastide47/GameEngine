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

using namespace Engine2D::Physics;

namespace Engine::Physics {
  class CollisionManifold {
    public:
      /// The first rigidbody involved in the collision
      std::shared_ptr<Rigidbody2D> rb1;
      /// The second rigidbody involved in the collision
      std::shared_ptr<Rigidbody2D> rb2;
      /// The normal along which the collision occurs
      Engine2D::Vector2f normal;
      /// The depth of the collision
      double depth;
      /// The first contact point between the two rigidbodies
      std::vector<Engine2D::Vector2f> contactPoints;

      CollisionManifold(
        const std::shared_ptr<Rigidbody2D> &rb1, const std::shared_ptr<Rigidbody2D> &rb2, Engine2D::Vector2f normal,
        double depth, Engine2D::Vector2f contactPoint1, Engine2D::Vector2f contactPoint2, uint8_t contactCount
      );
  };
}

#endif //COLLISION_MANIFOLD_H
