//
// Rigidbody2D.cpp
// Author: Antoine Bastide
// Date: 30/11/2024
//

#include "Engine2D/Physics/Rigidbody2D.hpp"
#include "Engine2D/Entity2D.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine/Settings.hpp"
#include "Engine2D/Physics/Collider2D.hpp"
#include "Engine2D/Types/Vector2.hpp"

namespace Engine2D::Physics {
  Rigidbody2D::Rigidbody2D()
    : isKinematic(false), affectedByGravity(true), linearVelocity(glm::vec2(0)), angularVelocity(0), angularDamping(1),
      staticFriction(0.6f), dynamicFriction(0.4f), mass(1), massInv(1), inertia(0), inertiaInv(0), force(glm::vec2(0)) {}

  void Rigidbody2D::SetMass(const float mass) {
    if (this->mass <= 0) {
      this->mass = 0;
      this->massInv = 0;
    } else {
      this->mass = mass;
      this->massInv = 1.0f / this->mass;
    }
  }

  float Rigidbody2D::GetMass() const {
    return this->mass;
  }

  void Rigidbody2D::NoFriction() {
    dynamicFriction = 0.0f;
    staticFriction = 0.0f;
  }

  void Rigidbody2D::DefaultFriction() {
    staticFriction = 0.6f;
    dynamicFriction = 0.4f;
  }

  void Rigidbody2D::step() {
    if (!IsActive() || isKinematic)
      return;

    // Apply half the velocity before and half after to make the movement more accurate on lower frame rates
    const float fixedDeltaTime = Engine::Settings::Physics::GetFixedDeltaTime();
    const auto gravityMod = glm::vec2(affectedByGravity);
    this->linearVelocity += (gravityMod * Engine::Settings::Physics::GetGravity() + force) * massInv * fixedDeltaTime * 0.5f;
    this->Transform()->UpdatePositionAndRotation(
      this->linearVelocity * fixedDeltaTime, this->angularVelocity * fixedDeltaTime * angularDamping
    );
    this->linearVelocity += (gravityMod * Engine::Settings::Physics::GetGravity() + force) * massInv * fixedDeltaTime * 0.5f;
    this->force = glm::vec2(0);
  }

  void Rigidbody2D::AddForce(const glm::vec2 &force) {
    this->force += force;
  }

  void Rigidbody2D::computeInertia(const std::shared_ptr<Collider2D> &collider) {
    switch (collider->type) {
      case Collider2D::None: inertia = inertiaInv = 0.0f;
        break;
      case Collider2D::Circle: inertia =
        0.5f * mass * Transform()->GetWorldHalfScale().x * Transform()->GetWorldHalfScale().x;
        inertiaInv = 1.0f / inertia;
        break;
      case Collider2D::Rectangle: inertia =
        RECTANGLE_COLLISION_FACTOR * mass * glm::dot(Transform()->GetWorldScale(), Transform()->GetWorldScale());
        inertiaInv = 1.0f / inertia;
        break;
      case Collider2D::Polygon: {
        float area = 0.0f;
        inertia = 0;
        for (int i = 0; i < collider->transformedVertices.size() - 1; ++i) {
          const float cross = glm::cross_2(collider->transformedVertices[i], collider->transformedVertices[i + 1]);
          area += cross;
          inertia += cross * (collider->transformedVertices[i].x * collider->transformedVertices[i].x + collider->
                              transformedVertices[i].x * collider->transformedVertices[i + 1].x + collider->
                              transformedVertices[i + 1].x * collider->transformedVertices[i + 1].x + collider->
                              transformedVertices[i].y * collider->transformedVertices[i].y + collider->transformedVertices[
                                i].y * collider->transformedVertices[i + 1].y + collider->transformedVertices[i + 1].y *
                              collider->transformedVertices[i + 1].y);
        }
        area = std::abs(area) * 0.5f;
        inertia = RECTANGLE_COLLISION_FACTOR * inertia / (area > 0 ? area : 1);
        inertiaInv = inertia > 0 ? 1.0f / inertia : 0;
        break;
      }
    }
  }
}
