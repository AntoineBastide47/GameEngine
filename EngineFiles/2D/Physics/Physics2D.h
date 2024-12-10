//
// Physics2D.h
// Author: Antoine Bastide
// Date: 29/11/2024
//

#ifndef PHYSICS2D_H
#define PHYSICS2D_H

#include <unordered_set>
#include <vector>

#include "CollisionManifold.h"
#include "2D/Types/Vector2.h"

namespace Engine2D {
  class Entity2D;
}

namespace Engine2D::Physics {
  class Rigidbody2D;
  class Physics2D {
    friend class Engine2D::Entity2D;
    public:
      static const Vector2 gravity;
      explicit Physics2D(float fixedDeltaTime);
      void step();
    private:
      float fixedDeltaTime;
      std::vector<Rigidbody2D *> rigidbodies;
      std::pmr::unordered_set<const Rigidbody2D *> rigidbodiesToRemove;
      std::vector<Engine::Physics::CollisionManifold> contactList;

      void addRigidBody(Rigidbody2D *rigidbody);
      void removeRigidbody(const Rigidbody2D *rigidbody);
      static void ResolveCollision(const Engine::Physics::CollisionManifold &contact);

  };
}

#endif //PHYSICS2D_H
