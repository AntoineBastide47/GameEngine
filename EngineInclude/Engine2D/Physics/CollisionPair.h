//
// CollisionPair.h
// Author: Antoine Bastide
// Date: 06/01/2025
//

#ifndef COLLISION_PAIR_H
#define COLLISION_PAIR_H

namespace Engine2D::Physics {
  class Collider2D;
  class Rigidbody2D;

  struct ContactPair {
    std::shared_ptr<Collider2D> collider1;
    std::shared_ptr<Collider2D> collider2;
    std::shared_ptr<Rigidbody2D> rigidbody1;
    std::shared_ptr<Rigidbody2D> rigidbody2;

    bool operator==(const ContactPair &other) const {
      return collider1 == other.collider1 &&
             collider2 == other.collider2 &&
             rigidbody1 == other.rigidbody1 &&
             rigidbody2 == other.rigidbody2;
    }
  };
}

#endif //COLLISION_PAIR_H
