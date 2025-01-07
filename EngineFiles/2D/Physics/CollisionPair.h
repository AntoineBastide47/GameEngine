//
// CollisionPair.h
// Author: Antoine Bastide
// Date: 06/01/2025
//

#ifndef COLLISION_PAIR_H
#define COLLISION_PAIR_H

#include <memory>

#include "2D/Physics/Collider2D.h"
#include "2D/Physics/Rigidbody2D.h"

namespace Engine2D::Physics {
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

template<> struct std::hash<ContactPair> {
  size_t operator()(const ContactPair &pair) const noexcept {
    const auto hash1 = std::hash<std::shared_ptr<Collider2D>>{}(pair.collider1);
    const auto hash2 = std::hash<std::shared_ptr<Collider2D>>{}(pair.collider2);
    const auto hash3 = std::hash<std::shared_ptr<Rigidbody2D>>{}(pair.rigidbody1);
    const auto hash4 = std::hash<std::shared_ptr<Rigidbody2D>>{}(pair.rigidbody2);
    return hash1 ^ (hash2 << 1) ^ (hash3 << 2) ^ (hash4 << 3);
  }
};

#endif //COLLISION_PAIR_H
