//
// Rigidbody2D.cpp
// Author: Antoine Bastide
// Date: 30/11/2024
//

#include "Engine2D/Physics/Rigidbody2D.h"
#include "Engine2D/Entity2D.h"
#include "Engine2D/Game2D.h"
#include "Common/Settings.h"
#include "Engine2D/Physics/Collider2D.h"

namespace Engine2D::Physics {
  Rigidbody2D::Rigidbody2D()
    : isKinematic(false), affectedByGravity(true), mass(
        Engine::Property{
          1.0f, [this] {
            if (this->mass < 0.0f)
              this->mass = 0;
            else if (this->mass == 0.0f)
              this->massInv = 0;
            else
              this->massInv = 1.0f / this->mass;
          }
        }
      ), angularVelocity(0), angularDamping(1), staticFriction(0.6f), dynamicFriction(0.4f), massInv(1), inertia(0), inertiaInv(0) {}

  void Rigidbody2D::NoFriction() {
    dynamicFriction = 0.0f;
    staticFriction = 0.0f;
  }

  void Rigidbody2D::DefaultFriction() {
    staticFriction = 0.6f;
    dynamicFriction = 0.4f;
  }

  void Rigidbody2D::step(const float fixedDeltaTime) {
    // Apply half the velocity before and half after to make the movement more accurate on lower frame rates
    this->linearVelocity +=
      (affectedByGravity * Engine::Settings::Physics::GetGravity() + force) * massInv * fixedDeltaTime * 0.5f;
    this->Transform()->position += this->linearVelocity * fixedDeltaTime;
    this->linearVelocity +=
      (affectedByGravity * Engine::Settings::Physics::GetGravity() + force) * massInv * fixedDeltaTime * 0.5f;

    this->Transform()->rotation += this->angularVelocity * fixedDeltaTime * angularDamping;
    this->force = Vector2f::Zero;
  }

  void Rigidbody2D::AddForce(const Vector2f &force) {
    this->force += force;
  }

  void Rigidbody2D::computeInertia(const std::shared_ptr<Collider2D> &collider) {
    switch (collider->type) {
      case Collider2D::None: inertia = inertiaInv = 0.0f;
        break;
      case Collider2D::Circle: inertia = 0.5f * mass * Transform()->WorldHalfScale().x * Transform()->WorldHalfScale().x;
        inertiaInv = 1.0f / inertia;
        break;
      case Collider2D::Rectangle: inertia = 1.0f / 12.0f * mass * Transform()->WorldScale() * Transform()->WorldScale();
        inertiaInv = 1.0f / inertia;
        break;
      case Collider2D::Polygon: {
        float area = 0.0f;
        inertia = 0;
        for (int i = 0; i < collider->transformedVertices.size() - 1; ++i) {
          const float cross = collider->transformedVertices[i] ^ collider->transformedVertices[i + 1];
          area += cross;
          inertia += cross * (collider->transformedVertices[i].x * collider->transformedVertices[i].x + collider->
                              transformedVertices[i].x * collider->transformedVertices[i + 1].x + collider->
                              transformedVertices[i + 1].x * collider->transformedVertices[i + 1].x + collider->
                              transformedVertices[i].y * collider->transformedVertices[i].y + collider->transformedVertices[
                                i].y * collider->transformedVertices[i + 1].y + collider->transformedVertices[i + 1].y *
                              collider->transformedVertices[i + 1].y);
        }
        area = std::abs(area) * 0.5f;
        inertia = (1.0f / 12.0f * inertia) / (area > 0 ? area : 1);
        inertiaInv = inertia > 0 ? 1.0f / inertia : 0;
        break;
      }
    }
  }
}
