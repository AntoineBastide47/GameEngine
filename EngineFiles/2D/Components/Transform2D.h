//
// Transform2D.h
// Author: Antoine Bastide
// Date: 03/11/2024
//

#ifndef TRANSFORM2D_H
#define TRANSFORM2D_H

#include "2D/Types/Vector2.h"

namespace Engine2D {
  /**
   * Represents the position, rotation, and scale of a 2D entity.
   * This class is used to define and manage the spatial transformations (position, rotation, scale)
   * of entities in a 2D space. It provides operators for equality comparisons between transforms.
   */
  class Transform2D {
    public:
      /** Position of the transform in 2D space. */
      Vector2 position;
      /** Rotation of the transform in degrees. */
      float rotation;
      /** Scale of the transform in 2D space. */
      Vector2 scale;

      /** Default constructor that initializes position to zero, rotation to 0.0f, and scale to one. */
      Transform2D();

      /**
       * @brief Parameterized constructor that initializes position, rotation, and scale to specified values.
       * @param position Initial position as a Vector2.
       * @param rotation Initial rotation in degrees.
       * @param scale Initial scale as a Vector2.
       */
      Transform2D(const Vector2 &position, const float &rotation, const Vector2 &scale);

      /** Equality operator to compare two Transform2D objects. */
      bool operator==(const Transform2D &other) const;
      /** Inequality operator to compare two Transform2D objects. */
      bool operator!=(const Transform2D &transform) const;
  };
} // namespace Engine2D

#endif // TRANSFORM2D_H
