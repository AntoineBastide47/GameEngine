//
// Collider2D.cpp
// Author: Antoine Bastide
// Date: 22/12/2024
//

#include "Engine2D/Physics/Collider2D.h"
#include "Engine2D/Entity2D.h"
#include "Engine2D/Transform2D.h"
#include "Engine2D/Physics/Rigidbody2D.h"
#include "Engine2D/Rendering/ShapeRenderer.h"
#include "Engine2D/Types/Vector2.h"

namespace Engine2D::Physics {
  Collider2D::Collider2D()
    : bounciness(1), offset(glm::vec2(0)), autoCompute(true), position(glm::vec2(0)), isTrigger(false), draw(false),
      type(None), initialized(false), lastModelMatrix() {}

  std::vector<glm::vec2> Collider2D::ContactPoints() const {
    return contactPoints;
  }

  Collider2D::Type Collider2D::GetType() const {
    return type;
  }

  Collider2D::AABB Collider2D::getAABB() {
    const auto matrix = Transform()->GetWorldMatrix();
    if (!rigidbody)
      rigidbody = Entity()->GetComponent<Rigidbody2D>();
    if (initialized && lastModelMatrix == matrix)
      return aabb;

    initialized = true;
    lastModelMatrix = matrix;
    computeAABB();
    return aabb;
  }

  void Collider2D::OnDrawGizmos2D() {
    if (!draw)
      return;

    if (type == Circle)
      Rendering::ShapeRenderer::DrawCircleWireframe(getPosition(), getScale().x, glm::vec4(0, 1, 0, 1), 32);
    else
      Rendering::ShapeRenderer::DrawPolygonWireframe(transformedVertices, glm::vec4(1, 0, 0, 1));
  }

  glm::vec2 Collider2D::getPosition() const {
    return (autoCompute ? Transform()->GetWorldPosition() : position) + offset;
  }

  CircleCollider2D::CircleCollider2D()
    : radius(0) {
    this->type = Circle;
  }

  void CircleCollider2D::computeAABB() {
    aabb.min = getPosition() - getScale();
    aabb.max = getPosition() + getScale();
  }

  glm::vec2 CircleCollider2D::getScale() {
    return autoCompute ? Transform()->GetWorldHalfScale() : glm::vec2(1) * radius;
  }

  RectangleCollider2D::RectangleCollider2D()
    : width(0), height(0) {
    this->type = Rectangle;
  }

  void RectangleCollider2D::computeAABB() {
    // Find the bounds of the rectangle
    const float left = -(autoCompute ? Transform()->GetWorldHalfScale().x : width);
    const float right = -left;
    const float top = autoCompute ? Transform()->GetWorldHalfScale().y : height;
    const float bottom = -top;

    // Transform the bounds of the rectangle
    this->transformedVertices = {
      glm::rotate_vector2_degrees(glm::vec2(left, top), Transform()->GetWorldRotation()) + getPosition(),
      glm::rotate_vector2_degrees(glm::vec2(right, top), Transform()->GetWorldRotation()) + getPosition(),
      glm::rotate_vector2_degrees(glm::vec2(right, bottom), Transform()->GetWorldRotation()) + getPosition(),
      glm::rotate_vector2_degrees(glm::vec2(left, bottom), Transform()->GetWorldRotation()) + getPosition(),
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

  glm::vec2 RectangleCollider2D::getScale() {
    return autoCompute ? Transform()->GetWorldScale() : glm::vec2(width, height);
  }

  PolygonCollider2D::PolygonCollider2D() {
    this->type = Polygon;
  }

  void PolygonCollider2D::computeAABB() {
    if (vertices.size() == 0) {
      aabb.min = glm::vec2(0);
      aabb.max = glm::vec2(0);
      return;
    }

    // Transform the bounds of the rectangle
    this->transformedVertices.resize(vertices.size());
    for (int i = 0; i < vertices.size(); ++i)
      this->transformedVertices[i] = glm::rotate_vector2_degrees(vertices[i], Transform()->GetWorldRotation()) +
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
