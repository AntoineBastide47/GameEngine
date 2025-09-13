//
// Rigidbody2D.cpp
// Author: Antoine Bastide
// Date: 30/11/2024
//

#include "Engine2D/Physics/Rigidbody2D.hpp"
#include "Engine2D/Entity2D.hpp"
#include "Engine/Settings.hpp"
#include "Engine2D/Physics/Collider2D.hpp"
#include "Engine2D/Types/Vector2.hpp"
#include "glm/gtx/norm.hpp"

namespace Engine2D::Physics {
  Rigidbody2D::Rigidbody2D()
    : isKinematic(false), affectedByGravity(true), linearVelocity(glm::vec2(0)), angularVelocity(0), angularDamping(1),
      staticFriction(0.6f), dynamicFriction(0.4f), mass(1), massInv(1), inertia(0), inertiaInv(0),
      force(glm::vec2(0)) {}

  void Rigidbody2D::SetMass(const float mass) {
    if (this->mass <= 0) {
      this->mass = 0;
      this->massInv = 0;
    } else {
      this->mass = mass;
      this->massInv = 1.0f / this->mass;
    }
  }

  float Rigidbody2D::Mass() const {
    return mass;
  }

  void Rigidbody2D::NoFriction() {
    dynamicFriction = 0.0f;
    staticFriction = 0.0f;
  }

  void Rigidbody2D::DefaultFriction() {
    staticFriction = 0.6f;
    dynamicFriction = 0.4f;
  }

  void Rigidbody2D::OnDeserialize(Engine::Reflection::Format format, const Engine::JSON &json) {
    SetMass(mass);
  }

  #if ENGINE_EDITOR
  bool Rigidbody2D::OnRenderInEditor(const std::string &name, const bool isConst, const bool readOnly) {
    bool changed = false;
    changed |= Engine::Reflection::_e_renderInEditorImpl(isKinematic, "Is Kinematic", readOnly);
    changed |= Engine::Reflection::_e_renderInEditorImpl(affectedByGravity, "Affected By Gravity", readOnly);
    changed |= Engine::Reflection::_e_renderInEditorImpl(mass, "Mass", readOnly);
    changed |= Engine::Reflection::_e_renderInEditorImpl(angularDamping, "Angular Damping", readOnly);
    changed |= Engine::Reflection::_e_renderInEditorImpl(staticFriction, "Static Friction", readOnly);
    changed |= Engine::Reflection::_e_renderInEditorImpl(dynamicFriction, "Dynamic Friction", readOnly);
    changed |= Engine::Reflection::_e_renderInEditorImpl(linearVelocity, "Linear Velocity", true);
    changed |= Engine::Reflection::_e_renderInEditorImpl(angularVelocity, "Angular Velocity", true);
    changed |= Engine::Reflection::_e_renderInEditorImpl(inertia, "Inertia", true);
    return changed;
  }

  void Rigidbody2D::OnEditorValueChanged() {
    SetMass(mass);
    inertiaInv = 1.0f / inertia;
  }
  #endif

  void Rigidbody2D::step() {
    if (!IsActive() || isKinematic)
      return;

    // Apply gravity
    const float dt = Engine::Settings::Physics::FixedDeltaTime();
    linearVelocity += (glm::vec2(affectedByGravity) * Engine::Settings::Physics::Gravity() + force) * massInv * dt;

    // Clamp velocities
    constexpr float maxVel = 120.0f;
    constexpr float maxVelSqr = maxVel * maxVel;
    if (const float linearVelSqr = glm::length2(linearVelocity); linearVelSqr > maxVelSqr)
      linearVelocity *= maxVelSqr / linearVelSqr;

    if (constexpr float maxAngularVelocity = 90.0f; std::abs(angularVelocity) > maxAngularVelocity)
      angularVelocity = (angularVelocity > 0 ? 1 : -1) * maxAngularVelocity;

    if (constexpr float minVel = 0.02f; glm::length(linearVelocity) < minVel && glm::length(force) > 0.0f)
      linearVelocity = glm::normalize(force) * minVel;

    Transform()->UpdatePositionAndRotation(linearVelocity * dt, angularVelocity * dt * angularDamping);
    force = {};
  }

  void Rigidbody2D::AddForce(const glm::vec2 &force) {
    this->force += force;
  }

  void Rigidbody2D::computeInertia(const Collider2D *collider) {
    switch (collider->type) {
      case Collider2D::None:
        inertia = inertiaInv = 0.0f;
        break;
      case Collider2D::Circle:
        inertia =
            0.5f * mass * Transform()->WorldHalfScale().x * Transform()->WorldHalfScale().x;
        break;
      case Collider2D::Rectangle:
        inertia =
            RECTANGLE_COLLISION_FACTOR * mass * glm::dot(Transform()->WorldScale(), Transform()->WorldScale());
        break;
      case Collider2D::Polygon: {
        float area = 0.0f;
        inertia = 0;
        for (size_t i = 0; i < collider->transformedVertices.size() - 1; ++i) {
          const float cross = glm::cross_2(collider->transformedVertices[i], collider->transformedVertices[i + 1]);
          area += cross;
          inertia += cross *
          (collider->transformedVertices[i].x * collider->transformedVertices[i].x +
           collider->transformedVertices[i].x * collider->transformedVertices[i + 1].x +
           collider->transformedVertices[i + 1].x * collider->transformedVertices[i + 1].x +
           collider->transformedVertices[i].y * collider->transformedVertices[i].y +
           collider->transformedVertices[i].y * collider->transformedVertices[i + 1].y +
           collider->transformedVertices[i + 1].y * collider->transformedVertices[i + 1].y);
        }
        area = std::abs(area) * 0.5f;
        inertia = RECTANGLE_COLLISION_FACTOR * inertia / (area > 0 ? area : 1);
        break;
      }
    }
    inertiaInv = inertia > 0 ? 1.0f / inertia : 0;
  }
}
