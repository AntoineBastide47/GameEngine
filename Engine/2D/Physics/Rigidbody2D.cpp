//
// Rigidbody2D.cpp
// Author: Antoine Bastide
// Date: 30/11/2024
//

#include "2D/Physics/Rigidbody2D.h"
#include "2D/Entity2D.h"

namespace Engine2D::Physics {
  Rigidbody2D::Rigidbody2D(
    const float density, const float mass, const float restitution, const float area, const bool isStatic,
    const float radius, const float width, const float height, const Rigidbody2DType type
  )
    : density(density), restitution(std::clamp(restitution, 0.0f, 1.0f)), isStatic(isStatic), radius(radius),
      width(width), height(height), rotationalVelocity(0), mass(0.0f), massInv(0.0f),
      area(area), type(type) {
    SetMass(mass);
  }

  std::vector<Vector2> Rigidbody2D::Vertices() const {
    if (type == Rectangle) {
      const float left = -Transform()->scale.x * 0.5f;
      const float right = -left;
      const float top = Transform()->scale.y * 0.5f;
      const float bottom = -top;

      return {
        Vector2(left, top).Rotated(Transform()->rotation) + Transform()->position,
        Vector2(right, top).Rotated(Transform()->rotation) + Transform()->position,
        Vector2(right, bottom).Rotated(Transform()->rotation) + Transform()->position,
        Vector2(left, bottom).Rotated(Transform()->rotation) + Transform()->position,
      };
    }
    return {};
  }

  void Rigidbody2D::Step(float fixedDeltaTime) {
    mass += restitution;
    mass -= restitution;
  }

  void Rigidbody2D::SetMass(const float mass) {
    this->mass = mass;
    this->massInv = 1.0f / mass;
  }

  float Rigidbody2D::GetMass() const {
    return this->mass;
  }

  Rigidbody2D::Rigidbody2DType Rigidbody2D::Type() const {
    return this->type;
  }

  Rigidbody2D Rigidbody2D::CreateCircleBody(
    const float radius, const float mass, const float density, const float restitution, const bool isStatic
  ) {
    return Rigidbody2D{
      density, mass, restitution, static_cast<float>(M_PI * 2.0f * radius), isStatic, radius, 0.0f, 0.0f, Circle
    };
  }

  Rigidbody2D Rigidbody2D::CreateRectangleBody(
    const float width, const float height, const float mass, const float density, const float restitution,
    const bool isStatic
  ) {
    return Rigidbody2D{density, mass, restitution, width * height, isStatic, 0.0f, width, height, Rectangle};
  }
}
