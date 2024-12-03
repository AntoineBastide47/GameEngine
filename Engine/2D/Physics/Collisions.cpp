//
// Collisions.cpp
// Author: Antoine Bastide
// Date: 29/11/2024
//

#include "2D/Physics/Collisions.h"

#include "2D/Entity2D.h"

namespace Engine2D::Physics {
  bool Collisions::CirclesIntersect(
    const Vector2 &centerA, const Vector2 &scaleA, const Vector2 &centerB, const Vector2 &scaleB, Vector2 *normal,
    float *depth
  ) {
    // Calculate the vector between the circle centers
    const float distance = Vector2::DistanceTo(centerA, centerB);
    const float combinedRadius = (scaleA.x + scaleB.x) * 0.5f;

    if (distance >= combinedRadius)
      return false;

    *normal = (centerB - centerA).Normalized();
    *depth = combinedRadius - distance;
    return true;
  }

  bool Collisions::PolygonsIntersect(
    const std::vector<Vector2> &verticesA, const Vector2 &positionA, const std::vector<Vector2> &verticesB,
    const Vector2 &positionB, Vector2 *normal, float *depth
  ) {
    float minA, maxA, minB, maxB, axisDepth;
    Vector2 pointA, pointB, axis;

    for (int i = 0; i < verticesA.size(); ++i) {
      pointA = verticesA[i];
      pointB = verticesA[(i + 1) % verticesA.size()];
      axis = (pointB - pointA).Perpendicular().Normalized();

      ProjectVertices(verticesA, axis, &minA, &maxA);
      ProjectVertices(verticesB, axis, &minB, &maxB);

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

      ProjectVertices(verticesA, axis, &minA, &maxA);
      ProjectVertices(verticesB, axis, &minB, &maxB);

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

  bool Collisions::PolygonAndCircleIntersect(
    const std::vector<Vector2> &polygonVertices, const Vector2 &polygonCenter, const Vector2 &circleCenter,
    const Vector2 &circleScale, Vector2 *normal, float *depth
  ) {
    float minA, maxA, minB, maxB, axisDepth;
    Vector2 axis;

    for (int i = 0; i < polygonVertices.size(); ++i) {
      Vector2 pointA = polygonVertices[i];
      Vector2 pointB = polygonVertices[(i + 1) % polygonVertices.size()];
      axis = (pointB - pointA).Perpendicular().Normalized();

      ProjectVertices(polygonVertices, axis, &minA, &maxA);
      ProjectCircle(circleCenter, circleScale, axis, &minB, &maxB);

      if (minA >= maxB || minB >= maxA)
        return false;

      if (axisDepth = std::min(maxB - minA, maxA - minB); axisDepth < *depth) {
        *depth = axisDepth;
        *normal = axis;
      }
    }

    const Vector2 closestPoint = ClosestPointOnPolygon(circleCenter, polygonVertices);
    axis = closestPoint - circleCenter;

    ProjectVertices(polygonVertices, axis, &minA, &maxA);
    ProjectCircle(circleCenter, circleScale, axis, &minB, &maxB);

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

  void Collisions::ProjectVertices(const std::vector<Vector2> &vertices, const Vector2 &axis, float *min, float *max) {
    *min = std::numeric_limits<float>::max();
    *max = -std::numeric_limits<float>::max();

    for (auto vertex: vertices) {
      const float projection = vertex * axis;
      *min = std::min(*min, projection);
      *max = std::max(*max, projection);
    }
  }

  void Collisions::ProjectCircle(
    const Vector2 &circleCenter, const Vector2 &circleScale, const Vector2 &axis, float *min, float *max
  ) {
    const Vector2 direction = axis.Normalized() * circleScale.x * 0.5f;

    *min = (circleCenter + direction) * axis;
    *max = (circleCenter - direction) * axis;

    if (*min > *max)
      std::swap(*min, *max);
  }

  Vector2 Collisions::ClosestPointOnPolygon(const Vector2 &point, const std::vector<Vector2> &vertices) {
    Vector2 result = Vector2::Zero;
    float min = std::numeric_limits<float>::max();

    for (auto vertex: vertices)
      if (const float distance = Vector2::DistanceTo(vertex, point); distance < min) {
        min = distance;
        result = vertex;
      }

    return result;
  }
}
