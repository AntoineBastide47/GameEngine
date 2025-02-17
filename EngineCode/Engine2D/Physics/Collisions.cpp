//
// Collisions.cpp
// Author: Antoine Bastide
// Date: 29/11/2024
//

#include "Engine2D/Physics/Collisions.h"
#include "Engine2D/Entity2D.h"
#include "Engine2D/Physics/Collider2D.h"
#include "Engine2D/Types/Vector2.h"

namespace Engine2D::Physics {
  bool Collisions::collideAABB(const Collider2D::AABB a, const Collider2D::AABB b) {
    return !(a.max.x <= b.min.x || b.max.x <= a.min.x || a.max.y <= b.min.y || b.max.y <= a.min.y);
  }

  bool Collisions::collide(
    const std::shared_ptr<Collider2D> &col1, const std::shared_ptr<Collider2D> &col2, glm::vec<2, double> *normal,
    double *depth
  ) {
    if (col1->type == Collider2D::Circle && col2->type == Collider2D::Circle)
      return circlesIntersect(
        col1->getPosition(), col1->getScale(), col2->getPosition(), col2->getScale(), normal, depth
      );
    if (col1->type == Collider2D::Circle && col2->type != Collider2D::Circle) {
      const bool collide = polygonAndCircleIntersect(
        col2->transformedVertices, col2->getPosition(), col1->getPosition(), col1->getScale(), normal, depth
      );
      *normal = -*normal;
      return collide;
    }
    if (col1->type != Collider2D::Circle && col2->type == Collider2D::Circle) {
      return polygonAndCircleIntersect(
        col1->transformedVertices, col1->getPosition(), col2->getPosition(), col2->getScale(), normal, depth
      );
    }
    return polygonsIntersect(
      col1->transformedVertices, col1->getPosition(), col2->transformedVertices, col2->getPosition(), normal, depth
    );
  }

  void Collisions::findContactPoints(
    const std::shared_ptr<Collider2D> &col1, const std::shared_ptr<Collider2D> &col2, glm::vec2 *contactPoint1,
    glm::vec2 *contactPoint2, uint8_t *contactCount
  ) {
    if (col1->type == Collider2D::Circle && col2->type == Collider2D::Circle) {
      findCirclesContactPoint(
        col1->getPosition(), col2->getPosition(), col1->Transform()->GetWorldHalfScale().x, contactPoint1
      );
      *contactCount = 1;
    } else if (col1->type == Collider2D::Circle && col2->type != Collider2D::Circle) {
      findCircleAndPolygonContactPoint(col1->getPosition(), col2->transformedVertices, contactPoint1);
      *contactCount = 1;
    } else if (col1->type != Collider2D::Circle && col2->type == Collider2D::Circle) {
      findCircleAndPolygonContactPoint(col2->getPosition(), col1->transformedVertices, contactPoint1);
      *contactCount = 1;
    } else
      findPolygonsContactPoint(
        col1->transformedVertices, col2->transformedVertices, contactPoint1, contactPoint2, contactCount
      );
  }

  bool Collisions::circlesIntersect(
    const glm::vec2 centerA, const glm::vec2 scaleA, const glm::vec2 centerB, const glm::vec2 scaleB,
    glm::vec<2, double> *normal, double *depth
  ) {
    // Calculate the vector between the circle centers
    const float distance = glm::distance(centerA, centerB);
    const float combinedRadius = scaleA.x + scaleB.x;

    if (distance >= combinedRadius)
      return false;

    *normal = glm::normalize(centerB - centerA);
    *depth = combinedRadius - distance;
    return true;
  }

  bool Collisions::polygonsIntersect(
    const std::vector<glm::vec2> &verticesA, const glm::vec2 positionA, const std::vector<glm::vec2> &verticesB,
    const glm::vec2 positionB, glm::vec<2, double> *normal, double *depth
  ) {
    double minA, maxA, minB, maxB, axisDepth;
    glm::vec2 pointA, pointB, axis;

    for (int i = 0; i < verticesA.size(); ++i) {
      pointA = verticesA[i];
      pointB = verticesA[(i + 1) % verticesA.size()];
      axis = glm::normalize(glm::perpendicular(pointB - pointA));

      projectVertices(verticesA, axis, &minA, &maxA);
      projectVertices(verticesB, axis, &minB, &maxB);

      if (minA >= maxB || minB >= maxA)
        return false;

      if (axisDepth = std::min(maxB - minA, maxA - minB); axisDepth < *depth) {
        *depth = axisDepth;
        *normal = axis;
      }
    }

    for (int i = 0; i < verticesB.size(); ++i) {
      pointA = verticesB[i];
      pointB = verticesB[(i + 1) % verticesB.size()];
      axis = glm::normalize(glm::perpendicular(pointB - pointA));

      projectVertices(verticesA, axis, &minA, &maxA);
      projectVertices(verticesB, axis, &minB, &maxB);

      if (minA >= maxB || minB >= maxA)
        return false;

      if (axisDepth = std::min(maxB - minA, maxA - minB); axisDepth < *depth) {
        *depth = axisDepth;
        *normal = axis;
      }
    }

    if (const glm::vec<2, double> direction = positionB - positionA; glm::dot(*normal, direction) < 0.0f)
      *normal *= -1.0f;

    return true;
  }

  bool Collisions::polygonAndCircleIntersect(
    const std::vector<glm::vec2> &polygonVertices, const glm::vec2 polygonCenter, const glm::vec2 circleCenter,
    const glm::vec2 circleHalfScale, glm::vec<2, double> *normal, double *depth
  ) {
    double minA, maxA, minB, maxB, axisDepth;
    glm::vec2 axis;

    for (int i = 0; i < polygonVertices.size(); ++i) {
      const glm::vec2 pointA = polygonVertices[i];
      const glm::vec2 pointB = polygonVertices[(i + 1) % polygonVertices.size()];
      axis = glm::normalize(glm::perpendicular(pointB - pointA));

      projectVertices(polygonVertices, axis, &minA, &maxA);
      projectCircle(circleCenter, circleHalfScale.x, axis, &minB, &maxB);

      if (minA >= maxB || minB >= maxA)
        return false;

      if (axisDepth = std::min(maxB - minA, maxA - minB); axisDepth < *depth) {
        *depth = axisDepth;
        *normal = axis;
      }
    }

    const glm::vec2 closestPoint = closestPointOnPolygon(circleCenter, polygonVertices);
    axis = glm::normalize(closestPoint - circleCenter);

    projectVertices(polygonVertices, axis, &minA, &maxA);
    projectCircle(circleCenter, circleHalfScale.x, axis, &minB, &maxB);

    if (minA >= maxB || minB >= maxA)
      return false;

    if (axisDepth = std::min(maxB - minA, maxA - minB); axisDepth < *depth) {
      *depth = axisDepth;
      *normal = axis;
    }

    if (const glm::vec<2, double> direction = polygonCenter - circleCenter; glm::dot(*normal, direction) < 0.0f)
      *normal *= -1.0f;

    return true;
  }

  void Collisions::projectVertices(const std::vector<glm::vec2> &vertices, const glm::vec2 axis, double *min, double *max) {
    *min = std::numeric_limits<double>::max();
    *max = std::numeric_limits<double>::lowest();

    for (const glm::vec2 vertex: vertices) {
      const double projection = glm::dot(vertex, axis);
      *min = std::min(*min, projection);
      *max = std::max(*max, projection);
    }
  }

  void Collisions::projectCircle(
    const glm::vec2 circleCenter, const float circleHalfScale, const glm::vec2 axis, double *min, double *max
  ) {
    const glm::vec2 direction = glm::normalize(axis) * circleHalfScale;
    *min = glm::dot(circleCenter + direction, axis);
    *max = glm::dot(circleCenter - direction, axis);

    if (*min > *max)
      std::swap(*min, *max);
  }

  glm::vec2 Collisions::closestPointOnPolygon(const glm::vec2 circleCenter, const std::vector<glm::vec2> &vertices) {
    auto closestPoint = glm::vec2(0);
    double minDistanceSquared = std::numeric_limits<float>::max();

    for (const auto vertex: vertices) {
      if (const double distance = glm::distance(vertex, circleCenter); distance < minDistanceSquared) {
        minDistanceSquared = distance;
        closestPoint = vertex;
      }
    }

    return closestPoint;
  }

  void Collisions::findCirclesContactPoint(
    const glm::vec2 centerA, const glm::vec2 centerB, const float radiusA, glm::vec2 *contactPoint
  ) {
    *contactPoint = centerA + glm::normalize(centerB - centerA) * radiusA;
  }

  void Collisions::findCircleAndPolygonContactPoint(
    const glm::vec2 circleCenter, const std::vector<glm::vec2> &vertices, glm::vec2 *contactPoint
  ) {
    double minDistanceSquared = std::numeric_limits<double>::max();
    for (int i = 0; i < vertices.size(); ++i) {
      const glm::vec2 v1 = vertices[i];
      const glm::vec2 v2 = vertices[(i + 1) % vertices.size()];

      double distanceSquared;
      glm::vec2 contact;
      pointSegmentDistance(circleCenter, v1, v2, &distanceSquared, &contact);

      if (distanceSquared < minDistanceSquared) {
        minDistanceSquared = distanceSquared;
        *contactPoint = contact;
      }
    }
  }

  void Collisions::findPolygonsContactPoint(
    const std::vector<glm::vec2> &verticesA, const std::vector<glm::vec2> &verticesB, glm::vec2 *contactPoint1,
    glm::vec2 *contactPoint2, uint8_t *contactCount
  ) {
    double minDistanceSquared = std::numeric_limits<double>::max();

    for (size_t i = 0; i < verticesA.size(); ++i) {
      const glm::vec2 p = verticesA[i];
      for (size_t j = 0; j < verticesB.size(); ++j) {
        const glm::vec2 va = verticesB[j];
        const glm::vec2 vb = verticesB[(j + 1) % verticesB.size()];

        double distanceSquared;
        glm::vec2 contact;
        pointSegmentDistance(p, va, vb, &distanceSquared, &contact);

        if (distanceSquared == minDistanceSquared && glm::approx_equals(contact, *contactPoint1)) {
          *contactCount = 2;
          *contactPoint2 = contact;
        } else if (distanceSquared < minDistanceSquared) {
          minDistanceSquared = distanceSquared;
          *contactCount = 1;
          *contactPoint1 = contact;
        }
      }
    }

    for (size_t i = 0; i < verticesB.size(); ++i) {
      const glm::vec2 p = verticesB[i];
      for (size_t j = 0; j < verticesA.size(); ++j) {
        const glm::vec2 va = verticesA[j];
        const glm::vec2 vb = verticesA[(j + 1) % verticesA.size()];

        double distanceSquared;
        glm::vec2 contact;
        pointSegmentDistance(p, va, vb, &distanceSquared, &contact);

        if (distanceSquared == minDistanceSquared && glm::approx_equals(contact, *contactPoint1)) {
          *contactCount = 2;
          *contactPoint2 = contact;
        } else if (distanceSquared < minDistanceSquared) {
          minDistanceSquared = distanceSquared;
          *contactCount = 1;
          *contactPoint1 = contact;
        }
      }
    }
  }

  void Collisions::pointSegmentDistance(
    const glm::vec2 p, const glm::vec2 pointA, const glm::vec2 pointB, double *distanceSquared, glm::vec2 *closestPoint
  ) {
    const glm::vec2 ab = pointB - pointA;
    const glm::vec2 ap = p - pointA;

    if (const double d = glm::dot(ab, ap) / glm::length_squared(ab); d < 0)
      *closestPoint = pointA;
    else if (d >= 1)
      *closestPoint = pointB;
    else
      *closestPoint = pointA + ab * glm::vec2(d);
    *distanceSquared = glm::distance_squared(p, *closestPoint);
  }
}
