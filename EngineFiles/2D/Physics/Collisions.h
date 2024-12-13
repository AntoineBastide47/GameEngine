//
// Collisions.h
// Author: Antoine Bastide
// Date: 29/11/2024
//

#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "Rigidbody2D.h"
#include "2D/Types/Vector2.h"

namespace Engine2D::Physics {
  class Collisions {
    friend class Physics2D;
    /**
     * Checks if the given rigidbodies AABB collide
     * @param a The first rigidbody AABB
     * @param b The second rigidbody AABB
     * @return True if the AABBs collide, false if not
     */
    [[nodiscard]] static bool collideAABB(Rigidbody2D::AABB a, Rigidbody2D::AABB b);
    /**
     * Checks if the given rigidbodies collide
     * @param rigidbodyA The first rigidbody
     * @param rigidbodyB The second rigidbody
     * @param normal The normal of the collision, calculated in the function's body
     * @param depth The depth of the collision, calculated in the function's body
     * @return True if they collide, false if not
     */
    [[nodiscard]] static bool collide(
      const Rigidbody2D *rigidbodyA, const Rigidbody2D *rigidbodyB, Vector2 *normal, float *depth
    );
    /**
     * Computes the number of contact points between two rigidbodies when they collide
     * @param rigidbodyA The first rigidbody
     * @param rigidbodyB The second rigidbody
     * @param contactPoint1 The first contact point, calculated in the function's body
     * @param contactPoint2 The first contact point, calculated in the function's body
     * @param contactCount The number of contact points, either 1 or 2, calculated in the function's body
     */
    static void findContactPoints(
      const Rigidbody2D *rigidbodyA, const Rigidbody2D *rigidbodyB, Vector2 *contactPoint1, Vector2 *contactPoint2,
      uint8_t *contactCount
    );

    /// True if the given circle bodies collide, False if not
    static bool circlesIntersect(
      const Vector2 &centerA, const Vector2 &scaleA, const Vector2 &centerB, const Vector2 &scaleB, Vector2 *normal,
      float *depth
    );
    /// True if the given polygon bodies collide, False if not
    static bool polygonsIntersect(
      const std::vector<Vector2> &verticesA, const Vector2 &positionA, const std::vector<Vector2> &verticesB,
      const Vector2 &positionB, Vector2 *normal, float *depth
    );
    /// True if the given polygon body and circle body collide, False if not
    static bool polygonAndCircleIntersect(
      const std::vector<Vector2> &polygonVertices, const Vector2 &polygonCenter, const Vector2 &circleCenter,
      const Vector2 &circleHalfScale, Vector2 *normal, float *depth
    );

    /// Projects the given vertices onto the given axis
    static void projectVertices(const std::vector<Vector2> &vertices, const Vector2 &axis, float *min, float *max);
    /// Projects the given circle onto the given axis
    static void projectCircle(
      const Vector2 &circleCenter, float circleHalfScale, const Vector2 &axis, float *min, float *max
    );
    /// Returns the closest point to the given point on the given polygon
    static Vector2 closestPointOnPolygon(const Vector2 &circleCenter, const std::vector<Vector2> &vertices);

    /// Finds the contact point between the given circles
    static void findCirclesContactPoint(
      const Vector2 &centerA, const Vector2 &centerB, float radiusA, Vector2 *contactPoint
    );
    /// Finds the contact point between the given circle and polygon
    static void findCircleAndPolygonContactPoint(
      const Vector2 &circleCenter, const std::vector<Vector2> &vertices, Vector2 *contactPoint
    );
    /// Finds the contact point(s) between the given polygons
    static void findPolygonsContactPoint(
      const std::vector<Vector2> &verticesA, const std::vector<Vector2> &verticesB, Vector2 *contactPoint1,
      Vector2 *contactPoint2, uint8_t *contactCount
    );

    /// Finds the distance and the closest point on the given segment between the given point and segment
    static void pointSegmentDistance(
      const Vector2 &p, const Vector2 &pointA, const Vector2 &pointB, float *distanceSquared, Vector2 *closestPoint
    );
  };
}

#endif //COLLISIONS_H
