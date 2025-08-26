//
// Collisions.cpp
// Author: Antoine Bastide
// Date: 29/11/2024
//

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#include "Engine2D/Physics/Collisions.hpp"
#include "Engine/Macros/Profiling.hpp"
#include "Engine2D/Transform2D.hpp"
#include "Engine2D/Physics/Collider2D.hpp"
#include "Engine2D/Types/Vector2.hpp"

namespace Engine2D::Physics {
  bool Collisions::collide(
    const Collider2D *col1, const Collider2D *col2, glm::vec<2, double> *normal, double *depth
  ) {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerSystem);

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
    const Collider2D *col1, const Collider2D *col2, glm::vec2 *contactPoint1, glm::vec2 *contactPoint2,
    uint8_t *contactCount
  ) {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerSystem);

    if (col1->type == Collider2D::Circle && col2->type == Collider2D::Circle) {
      findCirclesContactPoint(
        col1->getPosition(), col2->getPosition(), col1->Transform()->WorldHalfScale().x, contactPoint1
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
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerSubSystem);

    // Calculate the vector between the circle centers
    const float distance = glm::distance(centerA, centerB);
    const float combinedRadius = scaleA.x + scaleB.x;

    if (distance >= combinedRadius)
      return false;

    if (std::fabs(distance) < 1e-6f)
      *normal = {1, 0};
    else
      *normal = -glm::normalize(centerB - centerA);

    *depth = combinedRadius - distance;
    return true;
  }

  bool Collisions::polygonsIntersect(
    const std::vector<glm::vec2> &verticesA, const glm::vec2 positionA,
    const std::vector<glm::vec2> &verticesB, const glm::vec2 positionB,
    glm::vec<2, double> *normal, double *depth
  ) {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerSubSystem);

    const size_t countA = verticesA.size();
    const size_t countB = verticesB.size();
    if (countA == 0 || countB == 0)
      return false;

    *depth = std::numeric_limits<double>::max();
    double minA, maxA, minB, maxB, axisDepth;
    glm::vec2 pointA, pointB, axis;

    auto testAxes = [&](const std::vector<glm::vec2> &poly) -> bool {
      for (size_t i = 0; i < poly.size(); ++i) {
        pointA = poly[i];
        pointB = poly[(i + 1) % poly.size()];
        axis = glm::normalize(glm::perpendicular(pointB - pointA));

        projectVertices(verticesA, axis, &minA, &maxA);
        projectVertices(verticesB, axis, &minB, &maxB);

        if (minA >= maxB || minB >= maxA)
          return false;

        axisDepth = std::min(maxB - minA, maxA - minB);
        if (axisDepth < *depth) {
          *depth = axisDepth;
          *normal = axis;
        }
      }
      return true;
    };

    if (!testAxes(verticesA) || !testAxes(verticesB))
      return false;

    if (glm::dot(*normal, glm::vec<2, double>(positionB - positionA)) >= 0.0)
      *normal = -*normal;

    return true;
  }

  bool Collisions::polygonAndCircleIntersect(
    const std::vector<glm::vec2> &polygonVertices, const glm::vec2 polygonCenter, const glm::vec2 circleCenter,
    const glm::vec2 circleHalfScale, glm::vec<2, double> *normal, double *depth
  ) {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerSubSystem);

    double minA, maxA, minB, maxB, axisDepth;
    glm::vec2 axis;

    for (size_t i = 0; i < polygonVertices.size(); ++i) {
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

  void Collisions::projectVertices(
    const std::vector<glm::vec2> &vertices, const glm::vec2 axis, double *min, double *max
  ) {
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
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerFunction);

    auto closestPoint = glm::vec2(0);
    double minDistanceSquared = std::numeric_limits<float>::max();

    for (const auto vertex: vertices)
      if (const double distance = glm::distance(vertex, circleCenter); distance < minDistanceSquared) {
        minDistanceSquared = distance;
        closestPoint = vertex;
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
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerFunction);

    double minDistanceSquared = std::numeric_limits<double>::max();
    for (size_t i = 0; i < vertices.size(); ++i) {
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

  inline void segmentSegmentClosestPoints(
    const glm::vec<2, double> &p1, const glm::vec<2, double> &q1, const glm::vec<2, double> &p2,
    const glm::vec<2, double> &q2, double *dist2, glm::vec<2, double> *c1, glm::vec<2, double> *c2
  ) {
    const glm::vec<2, double> d1 = q1 - p1;
    const glm::vec<2, double> d2 = q2 - p2;
    const glm::vec<2, double> r = p1 - p2;

    const double a = glm::dot(d1, d1);
    const double e = glm::dot(d2, d2);
    const double f = glm::dot(d2, r);

    double s, t;

    if (a <= 1e-12 && e <= 1e-12) {
      s = t = 0.0;
    } else if (a <= 1e-12) {
      s = 0.0;
      t = glm::clamp(f / e, 0.0, 1.0);
    } else {
      const double c = glm::dot(d1, r);
      if (e <= 1e-12) {
        t = 0.0;
        s = glm::clamp(-c / a, 0.0, 1.0);
      } else {
        const double b = glm::dot(d1, d2);
        const double denom = a * e - b * b;
        s = denom == 0.0 ? 0.0 : glm::clamp((b * f - c * e) / denom, 0.0, 1.0);
        t = (b * s + f) / e;
        if (t < 0.0) {
          t = 0.0;
          s = glm::clamp(-c / a, 0.0, 1.0);
        } else if (t > 1.0) {
          t = 1.0;
          s = glm::clamp((b - c) / a, 0.0, 1.0);
        }
      }
    }

    *c1 = p1 + d1 * s;
    *c2 = p2 + d2 * t;
    *dist2 = glm::dot(*c1 - *c2, *c1 - *c2);
  }

  void Collisions::findPolygonsContactPoint(
    const std::vector<glm::vec2> &verticesA,
    const std::vector<glm::vec2> &verticesB,
    glm::vec2 *contactPoint1, glm::vec2 *contactPoint2,
    uint8_t *contactCount
  ) {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerFunction);

    constexpr double EPS = 1e-9;
    double minDist2 = std::numeric_limits<double>::max();
    *contactCount = 0;

    auto consider = [&](const double d2, const glm::vec2 &p) {
      if (d2 < minDist2 - EPS) {
        minDist2 = d2;
        *contactCount = 1;
        *contactPoint1 = p;
      } else if (std::abs(d2 - minDist2) <= EPS &&
                 glm::distance2(p, *contactPoint1) > EPS &&
                 *contactCount == 1) {
        *contactCount = 2;
        *contactPoint2 = p;
      }
    };

    const size_t nA = verticesA.size();
    const size_t nB = verticesB.size();

    for (size_t i = 0; i < nA; ++i) {
      const glm::vec2 a0 = verticesA[i];
      const glm::vec2 a1 = verticesA[(i + 1) % nA];

      for (size_t j = 0; j < nB; ++j) {
        const glm::vec2 b0 = verticesB[j];
        const glm::vec2 b1 = verticesB[(j + 1) % nB];

        double d2;
        glm::vec<2, double> cA, cB;
        segmentSegmentClosestPoints(a0, a1, b0, b1, &d2, &cA, &cB);
        consider(d2, 0.5 * (cA + cB));
      }
    }
  }

  void Collisions::pointSegmentDistance(
    const glm::vec2 p, const glm::vec2 pointA, const glm::vec2 pointB, double *distanceSquared, glm::vec2 *closestPoint
  ) {
    const glm::vec2 ab = pointB - pointA;
    const glm::vec2 ap = p - pointA;

    if (const double d = glm::dot(ab, ap) / glm::length2(ab); d < 0)
      *closestPoint = pointA;
    else if (d >= 1)
      *closestPoint = pointB;
    else
      *closestPoint = pointA + ab * glm::vec2(d);
    *distanceSquared = glm::distance_squared(p, *closestPoint);
  }
}
