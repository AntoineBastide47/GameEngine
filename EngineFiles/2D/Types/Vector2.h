//
// Vector2.h
// Vector2: Represents a 2D vector with common vector operations
// Author: Antoine Bastide
// Date: 03/11/2024
//

#ifndef VECTOR2_H
#define VECTOR2_H

#include <glm/glm.hpp>

namespace Engine2D {
  /**
   * @class Vector2
   * @brief Represents a 2D vector with x and y components and provides common vector operations.
   */
  class Vector2 {
    public:
      /** Default constructor initializes the vector to (0, 0). */
      Vector2();

      /**
       * @brief Constructs a vector with specified x and y components.
       * @param x The x-component of the vector.
       * @param y The y-component of the vector.
       */
      Vector2(float x, float y);

      /** Constant for a zero vector (0, 0). */
      static const Vector2 Zero;

      /** Constant for a unit vector (1, 1). */
      static const Vector2 One;

      /** Constant for a unit vector pointing to the right (1, 0). */
      static const Vector2 Right;

      /** Constant for a unit vector pointing to the left (-1, 0). */
      static const Vector2 Left;

      /** Constant for a unit vector pointing up (0, 1). */
      static const Vector2 Up;

      /** Constant for a unit vector pointing down (0, -1). */
      static const Vector2 Down;

      /**
       * @brief Negation operator, returns the vector with both components negated.
       * @return A new Vector2 that is the negation of this vector.
       */
      Vector2 operator-() const;

      /**
       * @brief Addition operator for adding two vectors.
       * @param v The vector to add.
       * @return A new Vector2 that is the sum of this vector and v.
       */
      Vector2 operator+(const Vector2 &v) const;

      /**
       * @brief Subtraction operator for subtracting two vectors.
       * @param v The vector to subtract.
       * @return A new Vector2 that is the difference of this vector and v.
       */
      Vector2 operator-(const Vector2 &v) const;

      /**
       * @brief Multiplication operator for component-wise vector multiplication.
       * @param v The vector to multiply.
       * @return A new Vector2 that is the component-wise product of this vector and v.
       */
      Vector2 operator*(const Vector2 &v) const;

      /**
       * @brief Division operator for component-wise vector division.
       * @param v The vector to divide.
       * @return A new Vector2 that is the component-wise quotient of this vector and v.
       */
      Vector2 operator/(const Vector2 &v) const;

      /**
       * @brief Addition assignment operator for component-wise addition.
       * @param v The vector to add.
       * @return A reference to this vector after addition.
       */
      Vector2 &operator+=(const Vector2 &v);

      /**
       * @brief Subtraction assignment operator for component-wise subtraction.
       * @param v The vector to subtract.
       * @return A reference to this vector after subtraction.
       */
      Vector2 &operator-=(const Vector2 &v);

      /**
       * @brief Multiplication assignment operator for component-wise multiplication.
       * @param v The vector to multiply.
       * @return A reference to this vector after multiplication.
       */
      Vector2 &operator*=(const Vector2 &v);

      /**
       * @brief Division assignment operator for component-wise division.
       * @param v The vector to divide.
       * @return A reference to this vector after division.
       */
      Vector2 &operator/=(const Vector2 &v);

      /**
       * @brief Scalar multiplication operator for scaling the vector.
       * @param s The scalar value to multiply by.
       * @return A new Vector2 that is this vector scaled by s.
       */
      Vector2 operator*(float s) const;

      /**
       * @brief Scalar division operator for scaling the vector.
       * @param s The scalar value to divide by.
       * @return A new Vector2 that is this vector scaled by 1/s.
       */
      Vector2 operator/(float s) const;

      /**
       * @brief Equality operator for comparing two vectors.
       * @param v The vector to compare with.
       * @return True if the vectors are equal; false otherwise.
       */
      bool operator==(const Vector2 &v) const;

      /**
       * @brief Inequality operator for comparing two vectors.
       * @param v The vector to compare with.
       * @return True if the vectors are not equal; false otherwise.
       */
      bool operator!=(const Vector2 &v) const;

      /**
       * @brief Converts this Vector2 to a glm::vec2 type for compatibility with OpenGL and GLM libraries.
       * @return A glm::vec2 representing the same vector.
       */
      glm::vec2 toGLM() const;

      /** The x-component of the vector. */
      float x;

      /** The y-component of the vector. */
      float y;
  };
} // namespace Engine2D

#endif // VECTOR2_H
