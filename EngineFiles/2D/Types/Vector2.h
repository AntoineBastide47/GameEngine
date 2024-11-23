//
// Vector2.h
// Author: Antoine Bastide
// Date: 03/11/2024
//

#ifndef VECTOR2_H
#define VECTOR2_H

#include <ostream>
#include <glm/glm.hpp>

namespace Engine2D {
  /** Represents a 2D vector with x and y components and provides common vector operations. */
  class Vector2 {
    public:
      /** The x-component of the vector. */
      float x;
      /** The y-component of the vector. */
      float y;

      /** Default constructor initializes the vector to (0, 0). */
      Vector2();
      /**
       * Constructs a vector with specified x and y components.
       * @param x The x-component of the vector.
       * @param y The y-component of the vector.
       */
      Vector2(float x, float y);
      /** Copy constructor */
      Vector2(const Vector2 &other) = default;

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

      /** Addition operator for adding two vectors. */
      Vector2 operator+(const Vector2 &v) const;
      /** Addition assignment operator for component-wise addition. */
      Vector2 &operator+=(const Vector2 &v);

      /** Negation operator, returns the vector with both components negated. */
      Vector2 operator-() const;
      /** Subtraction operator for subtracting two vectors. */
      Vector2 operator-(const Vector2 &v) const;
      /** Subtraction assignment operator for component-wise subtraction. */
      Vector2 &operator-=(const Vector2 &v);

      /** Multiplication operator for dot product. */
      float operator*(const Vector2 &v) const;

      /** Scalar multiplication operator for scaling the vector. */
      Vector2 operator*(float s) const;
      /** Scalar multiplication assignment for scaling the vector. */
      Vector2 &operator*=(float s);

      /** Modulo operator for scaling the vector. */
      Vector2 operator%(int s) const;
      /** Modulo assignment for scaling the vector. */
      Vector2 &operator%=(int s);

      /**
       * Scalar division operator for scaling the vector.
       * @note Calling this operator will log a warning if the scalar is equal to 0
       */
      Vector2 operator/(float s) const;
      /**
       * Scalar division assignment for scaling the vector.
       * @note Calling this operator will log a warning if the scalar is equal to 0
       */
      Vector2 &operator/=(float s);

      /**
       * Array operator to get the elements of the vector by index
       * @note Calling this operator will log a warning if the index is not in the range: [0-1]
       */
      float &operator[](int index);

      /** Assignment operator between a Vector2 and glm::vec2 */
      Vector2 &operator=(const glm::vec2 &other);
      /** Equality operator for comparing two vectors. */
      bool operator==(const Vector2 &v) const;
      /** Inequality operator for comparing two vectors. */
      bool operator!=(const Vector2 &v) const;

      /** Comparison operator that checks if the square magnitude of this vector is smaller that the square magnitude of vector v */
      bool operator<(const Vector2 &v) const;
      /** Comparison operator that checks if the square magnitude of this vector is greater that the square magnitude of vector v */
      bool operator>(const Vector2 &v) const;
      /** Comparison operator that checks if the square magnitude of this vector is smaller or equal that the square magnitude of vector v */
      bool operator<=(const Vector2 &v) const;
      /** Comparison operator that checks if the square magnitude of this vector is greater or equal that the square magnitude of vector v */
      bool operator>=(const Vector2 &v) const;

      /** << operator to allow easy printing of a Vector2 object. */
      friend std::ostream &operator<<(std::ostream &os, const Vector2 &vec);

      /** Converts the current vector to a string with this format: (x, y) */
      [[nodiscard]] std::string toString() const;

      /** @returns The magnitude/length/size of this vector */
      [[nodiscard]] float Magnitude() const;
      /** @returns The squared magnitude/length/size of this vector */
      [[nodiscard]] float SquareMagnitude() const;
      /** @returns The dot product between the current vector and v */
      [[nodiscard]] float Dot(const Vector2 &v) const;

      /** @returns The normalized version of the current vector with a magnitude of 1 */
      [[nodiscard]] Vector2 Normalized() const;
      /**
       * Clamp's the magnitude of the current vector to the given max value
       * @param max The maximum magnitude of this vector
       * @note Calling this method will log a warning if max <= 0
       * @return The clamped version of the current vector
       */
      [[nodiscard]] Vector2 ClampMagnitude(float max) const;
      /** @returns The component-wise multiplication */
      [[nodiscard]] Vector2 Scaled(const Vector2 &v) const;
      /** @returns The current vector rotated by the given amount of degrees */
      [[nodiscard]] Vector2 Rotated(float degrees) const;
      /** @returns A vector that is perpendicular to the current vector */
      [[nodiscard]] Vector2 Perpendicular() const;
      /**
       * Moves the current vector towards a target vector by a specified maximum distance.
       *
       * This function calculates a new vector that is at most `maxDistanceDelta` units
       * closer to the `target` vector. If the distance to the target is less than or
       * equal to `maxDistanceDelta`, it returns the target vector itself.
       *
       * @param target The target vector to move towards.
       * @param maxDistanceDelta The maximum distance the current vector is allowed to move
       *                         towards the target. If negative, the function will treat
       *                         this value as zero to prevent moving away from the target.
       * @return A new Vector2 that is the result of moving the current vector towards the
       *         target by at most `maxDistanceDelta`.
       */
      [[nodiscard]] Vector2 MoveTowards(const Vector2 &target, float maxDistanceDelta) const;

      /** Normalizes the current vector, giving it a magnitude of 1 */
      void Normalize();
      /** Scales the current vector with v by using component-wise multiplication */
      void Scale(const Vector2 &v);
      /** Rotates the current vector by the given amount of degrees */
      void Rotate(float degrees);

      /**
       * Converts this Vector2 to a glm::vec2 type for compatibility with OpenGL and GLM libraries.
       * @return A glm::vec2 representing the same vector.
       */
      [[nodiscard]] glm::vec2 toGLM() const;

      /**
       * Computes the <b>unsigned</b> angle between two vector
       * @param v1 The first vector
       * @param v2 The second vector
       * @return The angle between both vectors
       */
      [[nodiscard]] static float AngleBetween(const Vector2 &v1, const Vector2 &v2);
      /**
       * Computes the <b>signed</b> angle between two vector
       * @param v1 The first vector
       * @param v2 The second vector
       * @return The angle between both vectors
       */
      [[nodiscard]] static float SignedAngleBetween(const Vector2 &v1, const Vector2 &v2);
      /**
       * Computes the distance between two vector
       * @param current The first vector
       * @param target The second vector
       * @return The distance separating both vectors
       */
      [[nodiscard]] static float DistanceTo(const Vector2 &current, const Vector2 &target);

      /**
       * Clamp's the magnitude of the given vector to the given max value
       * @param target The vector to clamp
       * @param max The maximum magnitude of this vector
       * @note Calling this method will log a warning if max <= 0
       * @return The clamped version of the given vector
       */
      [[nodiscard]] static Vector2 ClampMagnitude(const Vector2 &target, float max);
      /**
       * Linearly interpolates between two vectors.
       *
       * This function returns a point along the line between the `current` and `target` vectors.
       * The parameter `t` determines how far to interpolate from `current` to `target`.
       * A `t` value of 0 returns `current`, a `t` value of 1 returns `target`, and values
       * in between produce a linear blend.
       *
       * @param current The starting vector.
       * @param target The target vector to interpolate towards.
       * @param t The interpolation factor, where 0 <= t <= 1.
       *          Values outside this range will extrapolate beyond the `current` and `target`.
       * @return A new Vector2 representing the interpolated position between `current` and `target`.
       */
      [[nodiscard]] static Vector2 Lerp(const Vector2 &current, const Vector2 &target, float t);
      /**
       * Linearly interpolates between two vectors without clamping.
       *
       * This function returns a point along the line between the `current` and `target` vectors.
       * The parameter `t` determines how far to interpolate from `current` to `target`.
       * Unlike the standard `Lerp` function, `t` is not clamped to the [0, 1] range,
       * which allows for extrapolation beyond `current` and `target`.
       *
       * @param current The starting vector.
       * @param target The target vector to interpolate towards.
       * @param t The interpolation factor. Values less than 0 will produce points before `current`,
       *          and values greater than 1 will produce points beyond `target`.
       * @return A new Vector2 representing the interpolated or extrapolated position
       *         based on `t` between `current` and `target`.
       */
      [[nodiscard]] static Vector2 LerpUnclamped(const Vector2 &current, const Vector2 &target, float t);
      /**
       * Moves a vector towards a target vector by a specified maximum distance.
       *
       * This function calculates a new vector that is at most `maxDistanceDelta` units
       * closer to the `target` vector. If the distance to the target is less than or
       * equal to `maxDistanceDelta`, it returns the target vector itself.
       *
       * @param initial The vector to move
       * @param target The target vector to move towards.
       * @param maxDistanceDelta The maximum distance the current vector is allowed to move
       *                         towards the target. If negative, the function will treat
       *                         this value as zero to prevent moving away from the target.
       * @return A new Vector2 that is the result of moving the initial vector towards the
       *         target by at most `maxDistanceDelta`.
       */
      [[nodiscard]] static Vector2 MoveTowards(const Vector2 &initial, const Vector2 &target, float maxDistanceDelta);
  };
} // namespace Engine2D

#endif // VECTOR2_H
