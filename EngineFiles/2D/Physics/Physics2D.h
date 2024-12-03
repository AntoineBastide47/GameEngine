//
// Physics2D.h
// Author: Antoine Bastide
// Date: 29/11/2024
//

#ifndef PHYSICS2D_H
#define PHYSICS2D_H

#include <vector>

#include "2D/Types/Vector2.h"

namespace Engine2D {
  class Entity2D;
}

namespace Engine2D::Physics {
  class Rigidbody2D;
  class Physics2D {
    friend class Rigidbody2D;
    friend class Engine2D::Entity2D;
    public:
      explicit Physics2D(float fixedDeltaTime);
      void simulate();
    private:
      float fixedDeltaTime;
      Vector2 normal;
      float depth;
      std::vector<Rigidbody2D *> rigidbodies;

      void addRigidBody(Rigidbody2D *rigidbody);
      void removeRigidbody(const Rigidbody2D *rigidbody);
      [[nodiscard]] static bool collide(
        const Rigidbody2D *rigidbodyA, const Rigidbody2D *rigidbodyB, Vector2 *normal, float *depth
      );
  };
}

#endif //PHYSICS2D_H
