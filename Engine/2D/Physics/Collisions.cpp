//
// Collisions.cpp
// Author: Antoine Bastide
// Date: 29/11/2024
//

#include "2D/Physics/Collisions.h"
#include "2D/Entity2D.h"
#include "2D/Physics/Rigidbody2D.h"

namespace Engine2D::Physics {
  bool Collisions::collideAABB(const Rigidbody2D::AABB a, const Rigidbody2D::AABB b) {
    return !(a.max.x <= b.min.x || b.max.x <= a.min.x || a.max.y <= b.min.y || b.max.y <= a.min.y);
  }

  bool Collisions::collide(const Rigidbody2D *rigidbodyA, const Rigidbody2D *rigidbodyB, Vector2 *normal, float *depth) {
    if (rigidbodyA->Type() == Rigidbody2D::Circle && rigidbodyB->Type() == Rigidbody2D::Circle)
      return circlesIntersect(
        rigidbodyA->Transform()->WorldPosition(), rigidbodyA->Transform()->WorldHalfScale(),
        rigidbodyB->Transform()->WorldPosition(), rigidbodyB->Transform()->WorldHalfScale(), normal, depth
      );
    if (rigidbodyA->Type() == Rigidbody2D::Circle && rigidbodyB->Type() != Rigidbody2D::Circle) {
      const bool collide = polygonAndCircleIntersect(
        rigidbodyB->transformedVertices, rigidbodyB->Transform()->WorldPosition(), rigidbodyA->Transform()->WorldPosition(),
        rigidbodyA->Transform()->WorldHalfScale(), normal, depth
      );
      *normal = -*normal;
      return collide;
    }
    if (rigidbodyA->Type() != Rigidbody2D::Circle && rigidbodyB->Type() == Rigidbody2D::Circle) {
      return polygonAndCircleIntersect(
        rigidbodyA->transformedVertices, rigidbodyA->Transform()->WorldPosition(), rigidbodyB->Transform()->WorldPosition(),
        rigidbodyB->Transform()->WorldHalfScale(), normal, depth
      );
    }
    return polygonsIntersect(
      rigidbodyA->transformedVertices, rigidbodyA->Transform()->WorldPosition(), rigidbodyB->transformedVertices,
      rigidbodyB->Transform()->WorldPosition(),
      normal, depth
    );
  }

  void Collisions::findContactPoints(
    const Rigidbody2D *rigidbodyA, const Rigidbody2D *rigidbodyB, Vector2 *contactPoint1, Vector2 *contactPoint2,
    uint8_t *contactCount
  ) {
    if (rigidbodyA->Type() == Rigidbody2D::Circle && rigidbodyB->Type() == Rigidbody2D::Circle) {
      findCirclesContactPoint(
        rigidbodyA->Transform()->WorldPosition(), rigidbodyB->Transform()->WorldPosition(),
        rigidbodyA->Transform()->WorldScale().x, contactPoint1
      );
      *contactCount = 1;
    }
    else if (rigidbodyA->Type() == Rigidbody2D::Circle && rigidbodyB->Type() != Rigidbody2D::Circle) {
      findCircleAndPolygonContactPoint(
        rigidbodyA->Transform()->WorldPosition(), rigidbodyB->transformedVertices,
        contactPoint1
      );
      *contactCount = 1;
    }
    else if (rigidbodyA->Type() != Rigidbody2D::Circle && rigidbodyB->Type() == Rigidbody2D::Circle) {
      findCircleAndPolygonContactPoint(
        rigidbodyB->Transform()->WorldPosition(), rigidbodyA->transformedVertices,
        contactPoint1
      );
      *contactCount = 1;
    }
    else {
      findPolygonsContactPoint(
        rigidbodyA->transformedVertices, rigidbodyB->transformedVertices, contactPoint1, contactPoint2, contactCount
      );
    }
  }

  bool Collisions::circlesIntersect(
    const Vector2 &centerA, const Vector2 &scaleA, const Vector2 &centerB, const Vector2 &scaleB, Vector2 *normal,
    float *depth
  ) {
    // Calculate the vector between the circle centers
    const float distance = Vector2::DistanceTo(centerA, centerB);
    const float combinedRadius = scaleA.x + scaleB.x;

    if (distance >= combinedRadius)
      return false;

    *normal = (centerB - centerA).Normalized();
    *depth = combinedRadius - distance;
    return true;
  }

  bool Collisions::polygonsIntersect(
    const std::vector<Vector2> &verticesA, const Vector2 &positionA, const std::vector<Vector2> &verticesB,
    const Vector2 &positionB, Vector2 *normal, float *depth
  ) {
    float minA, maxA, minB, maxB, axisDepth;
    Vector2 pointA, pointB, axis;

    for (int i = 0; i < verticesA.size(); ++i) {
      pointA = verticesA[i];
      pointB = verticesA[(i + 1) % verticesA.size()];
      axis = (pointB - pointA).Perpendicular().Normalized();

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
      axis = (pointB - pointA).Perpendicular().Normalized();

      projectVertices(verticesA, axis, &minA, &maxA);
      projectVertices(verticesB, axis, &minB, &maxB);

      if (minA >= maxB || minB >= maxA)
        return false;

      if (axisDepth = std::min(maxB - minA, maxA - minB); axisDepth < *depth) {
        *depth = axisDepth;
        *normal = axis;
      }
    }

    if (const Vector2 direction = positionB - positionA; *normal * direction < 0.0f)
      *normal *= -1.0f;

    return true;
  }

  bool Collisions::polygonAndCircleIntersect(
    const std::vector<Vector2> &polygonVertices, const Vector2 &polygonCenter, const Vector2 &circleCenter,
    const Vector2 &circleHalfScale, Vector2 *normal, float *depth
  ) {
    float minA, maxA, minB, maxB, axisDepth;
    Vector2 axis;

    for (int i = 0; i < polygonVertices.size(); ++i) {
      Vector2 pointA = polygonVertices[i];
      Vector2 pointB = polygonVertices[(i + 1) % polygonVertices.size()];
      axis = (pointB - pointA).Perpendicular().Normalized();

      projectVertices(polygonVertices, axis, &minA, &maxA);
      projectCircle(circleCenter, circleHalfScale.x, axis, &minB, &maxB);

      if (minA >= maxB || minB >= maxA)
        return false;

      if (axisDepth = std::min(maxB - minA, maxA - minB); axisDepth < *depth) {
        *depth = axisDepth;
        *normal = axis;
      }
    }

    const Vector2 closestPoint = closestPointOnPolygon(circleCenter, polygonVertices);
    axis = (closestPoint - circleCenter).Normalized();

    projectVertices(polygonVertices, axis, &minA, &maxA);
    projectCircle(circleCenter, circleHalfScale.x, axis, &minB, &maxB);

    if (minA >= maxB || minB >= maxA)
      return false;

    if (axisDepth = std::min(maxB - minA, maxA - minB); axisDepth < *depth) {
      *depth = axisDepth;
      *normal = axis;
    }

    if (const Vector2 direction = polygonCenter - circleCenter; *normal * direction < 0.0f)
      *normal *= -1.0f;

    return true;
  }

  void Collisions::projectVertices(const std::vector<Vector2> &vertices, const Vector2 &axis, float *min, float *max) {
    *min = std::numeric_limits<float>::max();
    *max = std::numeric_limits<float>::lowest();

    for (const Vector2 vertex: vertices) {
      const float projection = vertex * axis;
      *min = std::min(*min, projection);
      *max = std::max(*max, projection);
    }
  }

  void Collisions::projectCircle(
    const Vector2 &circleCenter, const float circleHalfScale, const Vector2 &axis, float *min, float *max
  ) {
    const Vector2 direction = axis.Normalized() * circleHalfScale;
    *min = (circleCenter + direction) * axis;
    *max = (circleCenter - direction) * axis;

    if (*min > *max)
      std::swap(*min, *max);
  }

  Vector2 Collisions::closestPointOnPolygon(const Vector2 &circleCenter, const std::vector<Vector2> &vertices) {
    Vector2 closestPoint;
    float minDistanceSquared = std::numeric_limits<float>::max();

    for (const auto vertex: vertices) {
      if (const float distance = Vector2::DistanceTo(vertex, circleCenter); distance < minDistanceSquared) {
        minDistanceSquared = distance;
        closestPoint = vertex;
      }
    }

    return closestPoint;
  }

  void Collisions::findCirclesContactPoint(
    const Vector2 &centerA, const Vector2 &centerB, const float radiusA, Vector2 *contactPoint
  ) {
    *contactPoint = centerA + (centerB - centerA).Normalized() * radiusA;
  }

  void Collisions::findCircleAndPolygonContactPoint(
    const Vector2 &circleCenter, const std::vector<Vector2> &vertices, Vector2 *contactPoint
  ) {
    float minDistanceSquared = std::numeric_limits<float>::max();
    for (int i = 0; i < vertices.size(); ++i) {
      const Vector2 &v1 = vertices[i];
      const Vector2 &v2 = vertices[(i + 1) % vertices.size()];

      float distanceSquared;
      Vector2 contact;
      pointSegmentDistance(circleCenter, v1, v2, &distanceSquared, &contact);

      if (distanceSquared < minDistanceSquared) {
        minDistanceSquared = distanceSquared;
        *contactPoint = contact;
      }
    }
  }

  void Collisions::findPolygonsContactPoint(
    const std::vector<Vector2> &verticesA, const std::vector<Vector2> &verticesB, Vector2 *contactPoint1,
    Vector2 *contactPoint2,
    uint8_t *contactCount
  ) {
    float minDistanceSquared = std::numeric_limits<float>::max();

    for (size_t i = 0; i < verticesA.size(); ++i) {
      const Vector2 &p = verticesA[i];
      for (size_t j = 0; j < verticesB.size(); ++j) {
        const Vector2 &va = verticesB[j];
        const Vector2 &vb = verticesB[(j + 1) % verticesB.size()];

        float distanceSquared;
        Vector2 contact;
        pointSegmentDistance(p, va, vb, &distanceSquared, &contact);

        if (distanceSquared == minDistanceSquared && Vector2::ApproxEquals(contact, *contactPoint1)) {
          *contactCount = 2;
          *contactPoint2 = contact;
        }
        else if (distanceSquared < minDistanceSquared) {
          minDistanceSquared = distanceSquared;
          *contactCount = 1;
          *contactPoint1 = contact;
        }
      }
    }

    for (size_t i = 0; i < verticesB.size(); ++i) {
      const Vector2 &p = verticesB[i];
      for (size_t j = 0; j < verticesA.size(); ++j) {
        const Vector2 &va = verticesA[j];
        const Vector2 &vb = verticesA[(j + 1) % verticesA.size()];

        float distanceSquared;
        Vector2 contact;
        pointSegmentDistance(p, va, vb, &distanceSquared, &contact);

        if (distanceSquared == minDistanceSquared && Vector2::ApproxEquals(contact, *contactPoint1)) {
          *contactCount = 2;
          *contactPoint2 = contact;
        }
        else if (distanceSquared < minDistanceSquared) {
          minDistanceSquared = distanceSquared;
          *contactCount = 1;
          *contactPoint1 = contact;
        }
      }
    }
  }

  void Collisions::pointSegmentDistance(
    const Vector2 &p, const Vector2 &pointA, const Vector2 &pointB, float *distanceSquared, Vector2 *closestPoint
  ) {
    const Vector2 ab = pointB - pointA;
    const Vector2 ap = p - pointA;

    if (const float d = ab * ap / ab.SquaredMagnitude(); d < 0)
      *closestPoint = pointA;
    else if (d >= 1)
      *closestPoint = pointB;
    else
      *closestPoint = pointA + ab * d;
    *distanceSquared = Vector2::SquaredDistanceTo(p, *closestPoint);
  }
}
