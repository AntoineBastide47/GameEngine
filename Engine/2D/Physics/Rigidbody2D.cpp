//
// Rigidbody2D.cpp
// Author: Antoine Bastide
// Date: 30/11/2024
//

#include <iostream>

#include "2D/Physics/Rigidbody2D.h"
#include "2D/Entity2D.h"
#include "2D/Game2D.h"
#include "2D/Physics/Physics2D.h"
#include "Common/Settings.h"

namespace Engine2D::Physics {
  Rigidbody2D::Rigidbody2D(
    const float mass, const float inertia, const float area, const bool isStatic, const float radius, const float width,
    const float height, const Rigidbody2DType type
  )
    : restitution(1), angularDamping(1), isStatic(isStatic), radius(radius), width(width),
      height(height), affectedByGravity(true), staticFriction(0.6f), dynamicFriction(0.4f), bindToViewportTop(false),
      bindToViewportBottom(false), bindToViewportLeft(false), bindToViewportRight(false), initialized(false),
      angularVelocity(0), massInv(0), inertia(inertia), inertiaInv(inertia > 0 ? 0 : 1.0f / inertia), area(area),
      type(type), lastModelMatrix() {
    this->mass = mass;
  }

  void Rigidbody2D::BindToViewport() {
    bindToViewportTop = true;
    bindToViewportBottom = true;
    bindToViewportLeft = true;
    bindToViewportRight = true;
  }

  void Rigidbody2D::UnbindFromViewport() {
    bindToViewportTop = false;
    bindToViewportBottom = false;
    bindToViewportLeft = false;
    bindToViewportRight = false;
  }

  void Rigidbody2D::NoFriction() {
    dynamicFriction = 0.0f;
    staticFriction = 0.0f;
  }

  void Rigidbody2D::DefaultFriction() {
    staticFriction = 0.6f;
    dynamicFriction = 0.4f;
  }

  Rigidbody2D::AABB Rigidbody2D::getAABB() {
    if (initialized && lastModelMatrix == Transform()->WorldMatrix())
      return aabb;
    if (type == Circle) {
      aabb.min = Transform()->WorldPosition() - Transform()->WorldScale();
      aabb.max = Transform()->WorldPosition() + Transform()->WorldScale();
    }
    else if (type == Rectangle) {
      // Find the bounds of the rectangle
      const float left = -Transform()->WorldHalfScale().x;
      const float right = -left;
      const float top = Transform()->WorldHalfScale().y;
      const float bottom = -top;

      // Transform the bounds of the rectangle
      this->transformedVertices = {
        Vector2(left, top).Rotated(Transform()->WorldRotation()) + Transform()->WorldPosition(),
        Vector2(right, top).Rotated(Transform()->WorldRotation()) + Transform()->WorldPosition(),
        Vector2(right, bottom).Rotated(Transform()->WorldRotation()) + Transform()->WorldPosition(),
        Vector2(left, bottom).Rotated(Transform()->WorldRotation()) + Transform()->WorldPosition(),
      };

      // Construct the AABB
      aabb.min = Vector2f::One * std::numeric_limits<float>::max();
      aabb.max = Vector2f::One * std::numeric_limits<float>::lowest();
      for (const auto vertex: transformedVertices) {
        aabb.min.x = std::min(aabb.min.x, vertex.x);
        aabb.min.y = std::min(aabb.min.y, vertex.y);
        aabb.max.x = std::max(aabb.max.x, vertex.x);
        aabb.max.y = std::max(aabb.max.y, vertex.y);
      }
    }
    initialized = true;
    lastModelMatrix = Transform()->WorldMatrix();
    return aabb;
  }

  void Rigidbody2D::step(const float fixedDeltaTime) {
    this->linearVelocity += (affectedByGravity * Engine::Settings::Physics.GetGravity() * massInv + force) * fixedDeltaTime;
    this->Transform()->position += this->linearVelocity * fixedDeltaTime;
    this->Transform()->rotation += this->angularVelocity * fixedDeltaTime * angularDamping;
    this->force = Vector2f::Zero;

    // Check if the rigidbody needs to be bound to the viewport
    if (bindToViewportTop || bindToViewportBottom || bindToViewportLeft || bindToViewportRight) {
      // Get half dimensions of the screen
      const float halfWidth = Game2D::ViewportWidth() * 0.5f;
      const float halfHeight = Game2D::ViewportHeight() * 0.5f;

      // Check for collisions with the left and right boundaries
      if (bindToViewportLeft && this->Transform()->position->x <= -halfWidth + this->Transform()->scale->x * 0.5f) {
        this->linearVelocity.x = -this->linearVelocity.x;
        this->Transform()->position->x = -halfWidth + this->Transform()->scale->x * 0.5f;
      }
      else if (bindToViewportRight && this->Transform()->position->x >= halfWidth - this->Transform()->scale->x * 0.5f) {
        this->linearVelocity.x = -this->linearVelocity.x;
        this->Transform()->position->x = halfWidth - this->Transform()->scale->x * 0.5f;
      }

      // Check for collisions with the top and bottom boundaries
      if (bindToViewportTop && this->Transform()->position->y <= -halfHeight + this->Transform()->scale->y * 0.5f) {
        this->linearVelocity.y = -this->linearVelocity.y;
        this->Transform()->position->y = -halfHeight + this->Transform()->scale->y * 0.5f;
      }
      else if (bindToViewportBottom && this->Transform()->position->y >= halfHeight - this->Transform()->scale->y * 0.5f) {
        this->linearVelocity.y = -this->linearVelocity.y;
        this->Transform()->position->y = halfHeight - this->Transform()->scale->y * 0.5f;
      }
    }
  }

  void Rigidbody2D::AddForce(const Vector2f &force) {
    this->force += force;
  }

  Rigidbody2D::Rigidbody2DType Rigidbody2D::Type() const {
    return this->type;
  }

  std::vector<Vector2f> Rigidbody2D::ContactPoints() const {
    return contactPoints;
  }

  Rigidbody2D *Rigidbody2D::CreateCircleBody(
    const float radius, const float mass, const bool isStatic
  ) {
    const float area = static_cast<float>(M_PI * radius * radius);
    const float inertia = 0.5f * mass * radius * radius;
    return new Rigidbody2D{
      mass, inertia, area, isStatic, radius, 0.0f, 0.0f, Circle
    };
  }

  Rigidbody2D *Rigidbody2D::CreateRectangleBody(
    const float width, const float height, const float mass, const bool isStatic
  ) {
    const float area = width * height;
    const float inertia = 1.0f / 12.0f * mass * (width * width + height * height);
    return new Rigidbody2D{mass, inertia, area, isStatic, 0.0f, width, height, Rectangle};
  }
}
