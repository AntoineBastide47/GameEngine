//
// Rigidbody2D.h
// Author: Antoine Bastide
// Date: 30/11/2024
//

#ifndef RIGIDBODY2D_H
#define RIGIDBODY2D_H

#include "2D/Components/Component2D.h"
#include "2D/Types/Vector2.h"
#include "Common/Property.h"

namespace Engine2D {
  class Game2D;
}

// TODO: find a way to draw the bounds without causing a drop in frame rate

namespace Engine2D::Physics {
  class Rigidbody2D : public Component2D {
    friend class Engine2D::Game2D;
    friend class Physics2D;
    public:
      enum Rigidbody2DType {
        Circle, Rectangle
      };
      float density{};
      float restitution{};
      bool isStatic{};
      float radius{};
      float width{};
      float height{};
      bool draw{};

      Rigidbody2D() = default;

      void Step(float fixedDeltaTime);
      [[nodiscard]] Rigidbody2DType Type() const;
      [[nodiscard]] std::vector<Vector2> Vertices() const;

      static Rigidbody2D CreateCircleBody(float radius, float mass, float density, float restitution, bool isStatic);
      static Rigidbody2D CreateRectangleBody(
        float width, float height, float mass, float density, float restitution, bool isStatic
      );
    private:
      Vector2 linearVelocity;
      float rotationalVelocity{};

      Engine::Property<float> mass{0, [this] { this->massInv = 1.0f / this->mass; }};
      float massInv{};
      float area{};
      Rigidbody2DType type{};

      Rigidbody2D(
        float density, float mass, float restitution, float area, bool isStatic, float radius, float width, float height,
        Rigidbody2DType type
      );
  };
}

#endif //RIGIDBODY2D_H
