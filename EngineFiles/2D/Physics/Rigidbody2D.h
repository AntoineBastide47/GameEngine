//
// Rigidbody2D.h
// Author: Antoine Bastide
// Date: 30/11/2024
//

#ifndef RIGIDBODY2D_H
#define RIGIDBODY2D_H

#include "2D/Components/Component2D.h"
#include "2D/Types/Vector2.h"

namespace Engine2D::Physics {
  class Rigidbody2D : public Component2D {
    public:
      enum Rigidbody2DType {
        Circle, Rectangle
      };
      float density;
      float restitution;
      bool isStatic;
      float radius;
      float width;
      float height;

      void Step(float fixedDeltaTime);
      void SetMass(float mass);
      [[nodiscard]] float GetMass() const;
      [[nodiscard]] Rigidbody2DType Type() const;
      [[nodiscard]] std::vector<Vector2> Vertices() const;

      static Rigidbody2D CreateCircleBody(float radius, float mass, float density, float restitution, bool isStatic);
      static Rigidbody2D CreateRectangleBody(
        float width, float height, float mass, float density, float restitution, bool isStatic
      );
    private:
      Vector2 linearVelocity;
      float rotationalVelocity;

      float mass;
      float massInv;
      float area;
      Rigidbody2DType type;

      Rigidbody2D(
        float density, float mass, float restitution, float area, bool isStatic, float radius, float width, float height,
        Rigidbody2DType type
      );
  };
}

#endif //RIGIDBODY2D_H
