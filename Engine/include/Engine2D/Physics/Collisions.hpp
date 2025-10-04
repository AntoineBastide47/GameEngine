//
// Collisions.h
// Author: Antoine Bastide
// Date: 29/11/2024
//

#ifndef COLLISIONS_H
#define COLLISIONS_H

#include <vector>
#include <glm/vec2.hpp>

#include "Engine/Types/Ptr.hpp"

namespace Engine2D::Physics {
  class Collider2D;

  class Collisions {
    friend class Physics2D;

    /// Checks if the given rigidbodies collide
    [[nodiscard]] static bool collide(
      const Engine::Ptr<Collider2D> &col1, const Engine::Ptr<Collider2D> &col2, glm::vec<2, double> *normal, double *depth
    );
    /// Computes the number of contact points between two rigidbodies when they collide
    static void findContactPoints(
      const Engine::Ptr<Collider2D> &col1, const Engine::Ptr<Collider2D> &col2, glm::vec2 *contactPoint1, glm::vec2 *contactPoint2,
      uint8_t *contactCount
    );

    /// True if the given circle bodies collide, False if not
    static bool circlesIntersect(
      glm::vec2 centerA, glm::vec2 scaleA, glm::vec2 centerB, glm::vec2 scaleB, glm::vec<2, double> *normal,
      double *depth
    );
    /// True if the given polygon bodies collide, False if not
    static bool polygonsIntersect(
      const std::vector<glm::vec2> &verticesA, glm::vec2 positionA, const std::vector<glm::vec2> &verticesB,
      glm::vec2 positionB, glm::vec<2, double> *normal, double *depth
    );
    /// True if the given polygon body and circle body collide, False if not
    static bool polygonAndCircleIntersect(
      const std::vector<glm::vec2> &polygonVertices, glm::vec2 polygonCenter, glm::vec2 circleCenter,
      glm::vec2 circleHalfScale, glm::vec<2, double> *normal, double *depth
    );

    /// Projects the given vertices onto the given axis
    static void projectVertices(const std::vector<glm::vec2> &vertices, glm::vec2 axis, double *min, double *max);
    /// Projects the given circle onto the given axis
    static void projectCircle(
      glm::vec2 circleCenter, float circleHalfScale, glm::vec2 axis, double *min, double *max
    );
    /// Returns the closest point to the given point on the given polygon
    static glm::vec2 closestPointOnPolygon(glm::vec2 circleCenter, const std::vector<glm::vec2> &vertices);

    /// Finds the contact point between the given circles
    static void findCirclesContactPoint(
      glm::vec2 centerA, glm::vec2 centerB, float radiusA, glm::vec2 *contactPoint
    );
    /// Finds the contact point between the given circle and polygon
    static void findCircleAndPolygonContactPoint(
      glm::vec2 circleCenter, const std::vector<glm::vec2> &vertices, glm::vec2 *contactPoint
    );
    /// Finds the contact point(s) between the given polygons
    static void findPolygonsContactPoint(
      const std::vector<glm::vec2> &verticesA, const std::vector<glm::vec2> &verticesB, glm::vec2 *contactPoint1,
      glm::vec2 *contactPoint2, uint8_t *contactCount
    );

    /// Finds the distance and the closest point on the given segment between the given point and segment
    static void pointSegmentDistance(
      glm::vec2 p, glm::vec2 pointA, glm::vec2 pointB, double *distanceSquared, glm::vec2 *closestPoint
    );
  };
}

#endif //COLLISIONS_H
