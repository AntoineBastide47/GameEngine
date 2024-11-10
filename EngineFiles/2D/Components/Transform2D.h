//
// Transform2D.h
// Transform2D: Represents the position, rotation, and scale of a 2D entity
// Author: Antoine Bastide
// Date: 03/11/2024
//

#ifndef TRANSFORM2D_H
#define TRANSFORM2D_H

#include "2D/Types/Vector2.h"

namespace Engine2D {
  /**
   * @class Transform2D
   * @brief Represents the position, rotation, and scale of a 2D entity.
   *
   * This class is used to define and manage the spatial transformations (position, rotation, scale)
   * of entities in a 2D space. It provides operators for equality comparisons between transforms.
   */
  class Transform2D {
    public:
      /**
       * @brief Default constructor that initializes the transform to default values.
       *
       * Initializes position to zero, rotation to 0.0f, and scale to one.
       */
      Transform2D();

      /**
       * @brief Parameterized constructor that initializes position, rotation, and scale to specified values.
       * @param position Initial position as a Vector2.
       * @param rotation Initial rotation in degrees.
       * @param scale Initial scale as a Vector2.
       */
      Transform2D(const Vector2 &position, const float &rotation, const Vector2 &scale);

      /**
       * @brief Equality operator to compare two Transform2D objects.
       * @param other The Transform2D object to compare with.
       * @return True if position, rotation, and scale are equal; false otherwise.
       */
      bool operator==(const Transform2D &other) const;

      /**
       * @brief Inequality operator to compare two Transform2D objects.
       * @param transform The Transform2D object to compare with.
       * @return True if any of position, rotation, or scale differ; false otherwise.
       */
      bool operator!=(const Transform2D &transform) const;

      /** Position of the transform in 2D space. */
      Vector2 position;

      /** Rotation of the transform in degrees. */
      float rotation;

      /** Scale of the transform in 2D space. */
      Vector2 scale;
  };
} // namespace Engine2D

#endif // TRANSFORM2D_H
