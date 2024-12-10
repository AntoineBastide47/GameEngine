//
// Rigidbody2D.cpp
// Author: Antoine Bastide
// Date: 30/11/2024
//

#include "2D/Physics/Rigidbody2D.h"
#include "2D/Entity2D.h"
#include "2D/Physics/Physics2D.h"
#include "Common/Log.h"

namespace Engine2D::Physics {
  Rigidbody2D::Rigidbody2D(
    const float density, const float mass, const float restitution, const float area, const bool isStatic,
    const float radius, const float width, const float height, const Rigidbody2DType type
  )
    : density(density), restitution(std::clamp(restitution, 0.0f, 1.0f)), isStatic(isStatic), radius(radius),
      width(width), height(height), affectedByGravity(true), area(area), type(type) {
    this->mass = mass;
  }

  std::vector<Vector2> Rigidbody2D::Vertices() {
    if (type == Rectangle) {
      const float left = -Transform()->scale->x * 0.5f;
      const float right = -left;
      const float top = Transform()->scale->y * 0.5f;
      const float bottom = -top;

      return this->vertices = {
        Vector2(left, top).Rotated(Transform()->rotation) + Transform()->position,
        Vector2(right, top).Rotated(Transform()->rotation) + Transform()->position,
        Vector2(right, bottom).Rotated(Transform()->rotation) + Transform()->position,
        Vector2(left, bottom).Rotated(Transform()->rotation) + Transform()->position,
      };
    }
    return this->vertices = {};
  }

  std::array<Vector2, 2> Rigidbody2D::GetAABB() {
    if (type == Circle) {
      aabbMin = Transform()->WorldPosition() - Transform()->WorldScale();
      aabbMax = Transform()->WorldPosition() + Transform()->WorldScale();
    }
    else {
      for (const auto vertices = this->vertices; auto &vertex: vertices) {
        if (vertex.x < aabbMin.x)
          aabbMin.x = vertex.x;
        if (vertex.x > aabbMax.x)
          aabbMax.x = vertex.x;
        if (vertex.y < aabbMin.y)
          aabbMin.y = vertex.y;
        if (vertex.y > aabbMax.y)
          aabbMax.y = vertex.y;
      }
    }
    return {aabbMin, aabbMax};
  }

  void Rigidbody2D::Step(const float fixedDeltaTime) {
    this->linearVelocity += (affectedByGravity * Physics2D::gravity * massInv + force) * fixedDeltaTime;
    this->Transform()->position += this->linearVelocity * fixedDeltaTime;
    this->Transform()->rotation += this->rotationalVelocity * fixedDeltaTime;
    this->force = Vector2::Zero;
  }

  void Rigidbody2D::AddForce(const Vector2 &force) {
    this->force += force;
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
