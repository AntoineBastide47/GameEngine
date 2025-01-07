//
// Collisions.cpp
// Author: Antoine Bastide
// Date: 29/11/2024
//

#include "2D/Physics/Collisions.h"
#include "2D/Entity2D.h"

namespace Engine2D::Physics {
  bool Collisions::collideAABB(const Collider2D::AABB a, const Collider2D::AABB b) {
    return !(a.max.x <= b.min.x || b.max.x <= a.min.x || a.max.y <= b.min.y || b.max.y <= a.min.y);
  }

  bool Collisions::collide(
    const std::shared_ptr<Collider2D> &rigidbodyA, const std::shared_ptr<Collider2D> &rigidbodyB, Vector2f *normal,
    double *depth
  ) {
    if (rigidbodyA->type == Collider2D::Circle && rigidbodyB->type == Collider2D::Circle)
      return circlesIntersect(
        rigidbodyA->Transform()->WorldPosition(), rigidbodyA->Transform()->WorldHalfScale(),
        rigidbodyB->Transform()->WorldPosition(), rigidbodyB->Transform()->WorldHalfScale(), normal, depth
      );
    if (rigidbodyA->type == Collider2D::Circle && rigidbodyB->type != Collider2D::Circle) {
      const bool collide = polygonAndCircleIntersect(
        rigidbodyB->transformedVertices, rigidbodyB->Transform()->WorldPosition(), rigidbodyA->Transform()->WorldPosition(),
        rigidbodyA->Transform()->WorldHalfScale(), normal, depth
      );
      *normal = -*normal;
      return collide;
    }
    if (rigidbodyA->type != Collider2D::Circle && rigidbodyB->type == Collider2D::Circle) {
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
    const std::shared_ptr<Collider2D> &rigidbodyA, const std::shared_ptr<Collider2D> &rigidbodyB, Vector2f *contactPoint1,
    Vector2f *contactPoint2, uint8_t *contactCount
  ) {
    if (rigidbodyA->type == Collider2D::Circle && rigidbodyB->type == Collider2D::Circle) {
      findCirclesContactPoint(
        rigidbodyA->Transform()->WorldPosition(), rigidbodyB->Transform()->WorldPosition(),
        rigidbodyA->Transform()->WorldHalfScale().x, contactPoint1
      );
      *contactCount = 1;
    }
    else if (rigidbodyA->type == Collider2D::Circle && rigidbodyB->type != Collider2D::Circle) {
      findCircleAndPolygonContactPoint(
        rigidbodyA->Transform()->WorldPosition(), rigidbodyB->transformedVertices,
        contactPoint1
      );
      *contactCount = 1;
    }
    else if (rigidbodyA->type != Collider2D::Circle && rigidbodyB->type == Collider2D::Circle) {
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
    const Vector2f centerA, const Vector2f scaleA, const Vector2f centerB, const Vector2f scaleB, Vector2f *normal,
    double *depth
  ) {
    // Calculate the vector between the circle centers
    const float distance = Vector2f::DistanceTo(centerA, centerB);
    const float combinedRadius = scaleA.x + scaleB.x;

    if (distance >= combinedRadius)
      return false;

    *normal = (centerB - centerA).Normalized();
    *depth = combinedRadius - distance;
    return true;
  }

  bool Collisions::polygonsIntersect(
    const std::vector<Vector2f> &verticesA, const Vector2f positionA, const std::vector<Vector2f> &verticesB,
    const Vector2f positionB, Vector2f *normal, double *depth
  ) {
    double minA, maxA, minB, maxB, axisDepth;
    Vector2f pointA, pointB, axis;

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

    if (const Vector2f direction = positionB - positionA; *normal * direction < 0.0f)
      *normal *= -1.0f;

    return true;
  }

  bool Collisions::polygonAndCircleIntersect(
    const std::vector<Vector2f> &polygonVertices, const Vector2f polygonCenter, const Vector2f circleCenter,
    const Vector2f circleHalfScale, Vector2f *normal, double *depth
  ) {
    double minA, maxA, minB, maxB, axisDepth;
    Vector2f axis;

    for (int i = 0; i < polygonVertices.size(); ++i) {
      const Vector2f pointA = polygonVertices[i];
      const Vector2f pointB = polygonVertices[(i + 1) % polygonVertices.size()];
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

    const Vector2f closestPoint = closestPointOnPolygon(circleCenter, polygonVertices);
    axis = (closestPoint - circleCenter).Normalized();

    projectVertices(polygonVertices, axis, &minA, &maxA);
    projectCircle(circleCenter, circleHalfScale.x, axis, &minB, &maxB);

    if (minA >= maxB || minB >= maxA)
      return false;

    if (axisDepth = std::min(maxB - minA, maxA - minB); axisDepth < *depth) {
      *depth = axisDepth;
      *normal = axis;
    }

    if (const Vector2f direction = polygonCenter - circleCenter; *normal * direction < 0.0f)
      *normal *= -1.0f;

    return true;
  }

  void Collisions::projectVertices(const std::vector<Vector2f> &vertices, const Vector2f axis, double *min, double *max) {
    *min = std::numeric_limits<double>::max();
    *max = std::numeric_limits<double>::lowest();

    for (const Vector2f vertex: vertices) {
      const double projection = vertex * axis;
      *min = std::min(*min, projection);
      *max = std::max(*max, projection);
    }
  }

  void Collisions::projectCircle(
    const Vector2f circleCenter, const float circleHalfScale, const Vector2f axis, double *min, double *max
  ) {
    const Vector2f direction = axis.Normalized() * circleHalfScale;
    *min = (circleCenter + direction) * axis;
    *max = (circleCenter - direction) * axis;

    if (*min > *max)
      std::swap(*min, *max);
  }

  Vector2f Collisions::closestPointOnPolygon(const Vector2f circleCenter, const std::vector<Vector2f> &vertices) {
    Vector2f closestPoint;
    double minDistanceSquared = std::numeric_limits<float>::max();

    for (const auto vertex: vertices) {
      if (const double distance = Vector2f::DistanceTo(vertex, circleCenter); distance < minDistanceSquared) {
        minDistanceSquared = distance;
        closestPoint = vertex;
      }
    }

    return closestPoint;
  }

  void Collisions::findCirclesContactPoint(
    const Vector2f centerA, const Vector2f centerB, const float radiusA, Vector2f *contactPoint
  ) {
    *contactPoint = centerA + (centerB - centerA).Normalized() * radiusA;
  }

  void Collisions::findCircleAndPolygonContactPoint(
    const Vector2f circleCenter, const std::vector<Vector2f> &vertices, Vector2f *contactPoint
  ) {
    double minDistanceSquared = std::numeric_limits<double>::max();
    for (int i = 0; i < vertices.size(); ++i) {
      const Vector2f v1 = vertices[i];
      const Vector2f v2 = vertices[(i + 1) % vertices.size()];

      double distanceSquared;
      Vector2f contact;
      pointSegmentDistance(circleCenter, v1, v2, &distanceSquared, &contact);

      if (distanceSquared < minDistanceSquared) {
        minDistanceSquared = distanceSquared;
        *contactPoint = contact;
      }
    }
  }

  void Collisions::findPolygonsContactPoint(
    const std::vector<Vector2f> &verticesA, const std::vector<Vector2f> &verticesB, Vector2f *contactPoint1,
    Vector2f *contactPoint2, uint8_t *contactCount
  ) {
    double minDistanceSquared = std::numeric_limits<double>::max();

    for (size_t i = 0; i < verticesA.size(); ++i) {
      const Vector2f p = verticesA[i];
      for (size_t j = 0; j < verticesB.size(); ++j) {
        const Vector2f va = verticesB[j];
        const Vector2f vb = verticesB[(j + 1) % verticesB.size()];

        double distanceSquared;
        Vector2f contact;
        pointSegmentDistance(p, va, vb, &distanceSquared, &contact);

        if (distanceSquared == minDistanceSquared && Vector2f::ApproxEquals(contact, *contactPoint1)) {
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
      const Vector2f p = verticesB[i];
      for (size_t j = 0; j < verticesA.size(); ++j) {
        const Vector2f va = verticesA[j];
        const Vector2f vb = verticesA[(j + 1) % verticesA.size()];

        double distanceSquared;
        Vector2f contact;
        pointSegmentDistance(p, va, vb, &distanceSquared, &contact);

        if (distanceSquared == minDistanceSquared && Vector2f::ApproxEquals(contact, *contactPoint1)) {
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
    const Vector2f p, const Vector2f pointA, const Vector2f pointB, double *distanceSquared, Vector2f *closestPoint
  ) {
    const Vector2f ab = pointB - pointA;
    const Vector2f ap = p - pointA;

    if (const double d = ab * ap / ab.SquaredMagnitude(); d < 0)
      *closestPoint = pointA;
    else if (d >= 1)
      *closestPoint = pointB;
    else
      *closestPoint = pointA + ab * d;
    *distanceSquared = Vector2f::SquaredDistanceTo(p, *closestPoint);
  }
}
