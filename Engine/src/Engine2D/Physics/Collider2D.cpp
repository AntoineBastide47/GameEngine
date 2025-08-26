//
// Collider2D.cpp
// Author: Antoine Bastide
// Date: 22/12/2024
//

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

#include "Engine2D/Physics/Collider2D.hpp"
#include "Engine2D/Entity2D.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/Physics/Physics2D.hpp"
#include "Engine2D/Physics/Rigidbody2D.hpp"
#include "Engine2D/SceneManagement/SceneManager.hpp"

namespace Engine2D::Physics {
  Collider2D::Collider2D()
    : elasticity(1), positionOffset(glm::vec2(0)), isTrigger(false), autoCompute(true), position(glm::vec2(0)),
      type(None), initialized(false), lastModelMatrix(), rigidbody() {}

  void Collider2D::forward() {
    Entity()->Scene()->physicsSystem.addCollider(this);
  }

  void Collider2D::recall() {
    Entity()->Scene()->physicsSystem.removeCollider(this);
  }

  std::vector<glm::vec2> Collider2D::ContactPoints() const {
    return contactPoints;
  }

  Collider2D::ColliderType Collider2D::Type() const {
    return type;
  }

  Collider2D::AABB Collider2D::getAABB() {
    const auto matrix = Transform()->WorldMatrix();
    if (!rigidbody)
      rigidbody = Entity()->GetComponent<Rigidbody2D>();
    if (initialized && lastModelMatrix == matrix)
      return aabb;

    initialized = true;
    lastModelMatrix = matrix;
    computeAABB();
    return aabb;
  }

  glm::vec2 Collider2D::getPosition() const {
    return (autoCompute ? Transform()->WorldPosition() : position) + positionOffset;
  }

  CircleCollider2D::CircleCollider2D()
    : radius(0) {
    type = Circle;
  }

  void CircleCollider2D::computeAABB() {
    aabb.min = getPosition() - getScale();
    aabb.max = getPosition() + getScale();
  }

  glm::vec2 CircleCollider2D::getScale() const {
    return autoCompute ? Transform()->WorldHalfScale() : glm::vec2(1) * radius;
  }

  BoxCollider2D::BoxCollider2D()
    : size(0) {
    type = Rectangle;
  }

  void BoxCollider2D::computeAABB() {
    // Find the bounds of the rectangle
    const float left = -(autoCompute ? Transform()->WorldHalfScale().x : size.x);
    const float right = -left;
    const float top = autoCompute ? Transform()->WorldHalfScale().y : size.y;
    const float bottom = -top;

    // Transform the bounds of the rectangle
    this->transformedVertices = {
      glm::rotate(glm::vec2(left, top), glm::radians(Transform()->WorldRotation())) + getPosition(),
      glm::rotate(glm::vec2(right, top), glm::radians(Transform()->WorldRotation())) + getPosition(),
      glm::rotate(glm::vec2(right, bottom), glm::radians(Transform()->WorldRotation())) + getPosition(),
      glm::rotate(glm::vec2(left, bottom), glm::radians(Transform()->WorldRotation())) + getPosition(),
    };

    // Construct the AABB
    aabb.min = glm::vec2(1) * std::numeric_limits<float>::max();
    aabb.max = glm::vec2(1) * std::numeric_limits<float>::lowest();
    for (const auto vertex: transformedVertices) {
      aabb.min.x = std::min(aabb.min.x, vertex.x);
      aabb.min.y = std::min(aabb.min.y, vertex.y);
      aabb.max.x = std::max(aabb.max.x, vertex.x);
      aabb.max.y = std::max(aabb.max.y, vertex.y);
    }
  }

  glm::vec2 BoxCollider2D::getScale() const {
    return autoCompute ? Transform()->WorldScale() : size;
  }

  PolygonCollider2D::PolygonCollider2D() {
    type = Polygon;
  }

  void PolygonCollider2D::computeAABB() {
    if (vertices.size() == 0) {
      aabb.min = glm::vec2(0);
      aabb.max = glm::vec2(0);
      return;
    }

    // Transform the bounds of the rectangle
    this->transformedVertices.resize(vertices.size());
    for (size_t i = 0; i < vertices.size(); ++i)
      this->transformedVertices[i] = glm::rotate(vertices[i], glm::radians(Transform()->WorldRotation())) +
                                     getPosition();

    // Construct the AABB
    aabb.min = glm::vec2(1) * std::numeric_limits<float>::max();
    aabb.max = glm::vec2(1) * std::numeric_limits<float>::lowest();
    for (const auto vertex: transformedVertices) {
      aabb.min.x = std::min(aabb.min.x, vertex.x);
      aabb.min.y = std::min(aabb.min.y, vertex.y);
      aabb.max.x = std::max(aabb.max.x, vertex.x);
      aabb.max.y = std::max(aabb.max.y, vertex.y);
    }
  }
}
