//
// Collider2D.cpp
// Author: Antoine Bastide
// Date: 22/12/2024
//

#include "2D/Physics/Collider2D.h"
#include "2D/Entity2D.h"
#include "2D/Transform2D.h"
#include "2D/Physics/Rigidbody2D.h"

namespace Engine2D::Physics {
  Collider2D::Collider2D() : bounciness(1), isTrigger(false), type(None), initialized(false), lastModelMatrix() {}

  std::vector<Vector2f> Collider2D::ContactPoints() const {
    return contactPoints;
  }

  Collider2D::Type Collider2D::GetType() const {
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

  CircleCollider2D::CircleCollider2D() {
    this->type = Circle;
  }

  void CircleCollider2D::computeAABB() {
    aabb.min = Transform()->WorldPosition() - Transform()->WorldHalfScale();
    aabb.max = Transform()->WorldPosition() + Transform()->WorldHalfScale();
  }

  RectangleCollider2D::RectangleCollider2D() {
    this->type = Rectangle;
  }

  void RectangleCollider2D::computeAABB() {
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

  ShapeCollider2D::ShapeCollider2D() {
    this->type = Shape;
  }

  void ShapeCollider2D::computeAABB() {
    if (vertices.size() == 0) {
      aabb.min = Vector2f::Zero;
      aabb.max = Vector2f::Zero;
      return;
    }

    // Transform the bounds of the rectangle
    this->transformedVertices.resize(vertices.size());
    for (int i = 0; i < vertices.size(); ++i)
      this->transformedVertices[i] = vertices[i].Rotated(Transform()->WorldRotation()) + Transform()->WorldPosition();

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
}
