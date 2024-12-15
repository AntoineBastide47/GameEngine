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
      const Rigidbody2D *rigidbodyA, const Rigidbody2D *rigidbodyB, Vector2f *normal, float *depth
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
      const Rigidbody2D *rigidbodyA, const Rigidbody2D *rigidbodyB, Vector2f *contactPoint1, Vector2f *contactPoint2,
      uint8_t *contactCount
    );

    /// True if the given circle bodies collide, False if not
    static bool circlesIntersect(
      Vector2f centerA, Vector2f scaleA, Vector2f centerB, Vector2f scaleB, Vector2f *normal, float *depth
    );
    /// True if the given polygon bodies collide, False if not
    static bool polygonsIntersect(
      const std::vector<Vector2f> &verticesA, Vector2f positionA, const std::vector<Vector2f> &verticesB,
      Vector2f positionB, Vector2f *normal, float *depth
    );
    /// True if the given polygon body and circle body collide, False if not
    static bool polygonAndCircleIntersect(
      const std::vector<Vector2f> &polygonVertices, Vector2f polygonCenter, Vector2f circleCenter,
      Vector2f circleHalfScale, Vector2f *normal, float *depth
    );

    /// Projects the given vertices onto the given axis
    static void projectVertices(const std::vector<Vector2f> &vertices, Vector2f axis, float *min, float *max);
    /// Projects the given circle onto the given axis
    static void projectCircle(
      Vector2f circleCenter, float circleHalfScale, Vector2f axis, float *min, float *max
    );
    /// Returns the closest point to the given point on the given polygon
    static Vector2f closestPointOnPolygon(Vector2f circleCenter, const std::vector<Vector2f> &vertices);

    /// Finds the contact point between the given circles
    static void findCirclesContactPoint(
      Vector2f centerA, Vector2f centerB, float radiusA, Vector2f *contactPoint
    );
    /// Finds the contact point between the given circle and polygon
    static void findCircleAndPolygonContactPoint(
      Vector2f circleCenter, const std::vector<Vector2f> &vertices, Vector2f *contactPoint
    );
    /// Finds the contact point(s) between the given polygons
    static void findPolygonsContactPoint(
      const std::vector<Vector2f> &verticesA, const std::vector<Vector2f> &verticesB, Vector2f *contactPoint1,
      Vector2f *contactPoint2, uint8_t *contactCount
    );

    /// Finds the distance and the closest point on the given segment between the given point and segment
    static void pointSegmentDistance(
      Vector2f p, Vector2f pointA, Vector2f pointB, float *distanceSquared, Vector2f *closestPoint
    );
  };
}

#endif //COLLISIONS_H
