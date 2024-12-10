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
    public:
      [[nodiscard]] static bool collide(Rigidbody2D *rigidbodyA, Rigidbody2D *rigidbodyB, Vector2 *normal, float *depth);
      static void FindContactPoints(
        Rigidbody2D *a, Rigidbody2D *b, Vector2 *contactPoint1, Vector2 *contactPoint2, int *contactCount
      );
      static bool CirclesIntersect(
        const Vector2 &centerA, const Vector2 &scaleA, const Vector2 &centerB, const Vector2 &scaleB, Vector2 *normal,
        float *depth
      );
      static bool PolygonsIntersect(
        const std::vector<Vector2> &verticesA, const Vector2 &positionA, const std::vector<Vector2> &verticesB,
        const Vector2 &positionB, Vector2 *normal, float *depth
      );
      static bool PolygonAndCircleIntersect(
        const std::vector<Vector2> &polygonVertices, const Vector2 &polygonCenter, const Vector2 &circleCenter,
        const Vector2 &
        circleScale, Vector2 *normal, float *depth
      );
      static void FindContactPoint(
        const Vector2 &centerA, const Vector2 &centerB, float radiusA, Vector2 *contactPoint
      );
    private:
      static float getPolygonRadius(const std::vector<Vector2> &polygonVertices, const Vector2 &polygonCenter);
      static void projectVertices(const std::vector<Vector2> &vertices, const Vector2 &axis, float *min, float *max);
      static void projectCircle(
        const Vector2 &circleCenter, const Vector2 &circleScale, const Vector2 &axis, float *min, float *max
      );
      static Vector2 closestPointOnPolygon(const Vector2 &point, const std::vector<Vector2> &vertices);
  };
}

#endif //COLLISIONS_H
