//
// Collisions.h
// Author: Antoine Bastide
// Date: 29/11/2024
//

#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "2D/Types/Vector2.h"

namespace Engine2D::Physics {
  class Collisions {
    public:
      static bool CirclesIntersect(const Vector2 &centerA, const Vector2 &scaleA, const Vector2 &centerB, const Vector2 &scaleB, Vector2 *normal, float *depth);
      static bool PolygonsIntersect(
        const std::vector<Vector2> &verticesA, const Vector2 &positionA, const std::vector<Vector2> &verticesB,
        const Vector2 &positionB, Vector2 *normal, float *depth
      );
      static bool PolygonAndCircleIntersect(
        const std::vector<Vector2> &polygonVertices, const Vector2 &polygonCenter, const Vector2 &circleCenter, const Vector2 &
        circleScale, Vector2 *normal, float
        *depth
      );
    private:
      static void ProjectVertices(const std::vector<Vector2> &vertices, const Vector2 &axis, float *min, float *max);
      static void ProjectCircle(const Vector2 &circleCenter, const Vector2 &circleScale, const Vector2 &axis, float *min, float *max);
      static Vector2 ClosestPointOnPolygon(const Vector2 &point, const std::vector<Vector2> &vertices);
  };
}

#endif //COLLISIONS_H
