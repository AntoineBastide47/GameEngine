//
// Vector2.h
// Author: Antoine Bastide
// Date: 03/11/2024
//

#ifndef VECTOR2_H
#define VECTOR2_H

#include <algorithm>
#include <iostream>
#include <string>
#include <glm/glm.hpp>

#include "2D/Types/Vector2.h"
#include "Common/Log.h"

namespace Engine2D {
  /** Represents a 2D vector with x and y components and provides common vector operations. */
  template<typename T = float> class Vector2 {
    static_assert(std::is_arithmetic_v<T>, "Vector2 requires an arithmetic type");
    public:
      /// The x-component of the vector.
      T x;
      /// The y-component of the vector.
      T y;

      /// Default constructor initializes the vector to (0, 0).
      Vector2() : x(0), y(0) {}
      /**
       * Constructs a vector with the specified x and y components.
       * @param x The x-component of the vector.
       * @param y The y-component of the vector.
       */
      Vector2(T x, T y) : x(x), y(y) {}

      /// Constant for a zero vector (0, 0).
      static const Vector2 Zero;
      /// Constant for a unit vector (1, 1).
      static const Vector2 One;
      /// Constant for a unit vector pointing to the right (1, 0).
      static const Vector2 Right;
      /// Constant for a unit vector pointing to the left (-1, 0).
      static const Vector2 Left;
      /// Constant for a unit vector pointing up (0, 1).
      static const Vector2 Up;
      /// Constant for a unit vector pointing down (0, -1).
      static const Vector2 Down;

      /// Addition operator for component-wise addition.
      constexpr Vector2 operator+(const Vector2 v) const {
        return {x + v.x, y + v.y};
      }

      /// Addition assignment operator for component-wise addition.
      constexpr Vector2 &operator+=(const Vector2 v) {
        return *this = *this + v;
      }

      /// Negation operator, returns the vector with all it's components negated.
      constexpr Vector2 operator-() const {
        return {-x, -y};
      }

      /// Subtraction operator for subtracting two vectors.
      constexpr Vector2 operator-(const Vector2 v) const {
        return {x - v.x, y - v.y};
      }

      /// Subtraction assignment operator for component-wise subtraction.
      constexpr Vector2 &operator-=(const Vector2 v) {
        return *this = *this - v;
      }

      /// Multiplication operator for dot product.
      constexpr float operator*(const Vector2 v) const {
        return x * v.x + y * v.y;
      }

      /// Scalar multiplication operator for scaling the vector.
      constexpr Vector2 operator*(const float scalar) const {
        return {x * scalar, y * scalar};
      }

      /// Scalar multiplication operator for scaling the vector.
      friend constexpr Vector2 operator*(const float scalar, const Vector2 vec) {
        return vec * scalar;
      }

      /// Scalar multiplication assignment for scaling the vector.
      constexpr Vector2 &operator*=(const float scalar) {
        return *this = *this * scalar;
      }

      /// Cross product operator
      constexpr float operator^(const Vector2 v) const {
        return x * v.y - y * v.x;
      }

      /**
       * Scalar division operator for scaling the vector.
       * @note Calling this operator will log a warning if the scalar is equal to 0
       */
      Vector2 operator/(const float scalar) const {
        if (scalar == 0.0f) {
          LOG_WARNING("Can not divide a Vector2 by 0, resulting operation will default to not modify the Vector2 instance.");
          return *this;
        }
        return *this * (1 / scalar);
      }

      /**
       * Scalar division assignment for scaling the vector.
       * @note Calling this operator will log a warning if the scalar is equal to 0
       */
      Vector2 &operator/=(const float scalar) {
        if (scalar == 0.0f) {
          LOG_WARNING("Can not divide a Vector2 by 0, resulting operation will default to not modify the Vector2 instance.");
          return *this;
        }
        return *this = *this * (1 / scalar);
      }

      /**
       * Array operator to get the elements of the vector by index
       * @note Calling this operator will log a warning if the index is not in the range: [0-1]
       */
      T &operator[](const int index) {
        if (index == 0)
          return x;
        if (index == 1)
          return y;
        LOG_WARNING(
          "Vector2 index out of range, resulting operation will default to returning the x component of the Vector2 instance."
        );
        return x;
      }

      /// Assignment operator between a Vector2 and glm::vec2
      constexpr Vector2 &operator=(const glm::vec2 other) {
        x = other.x;
        y = other.y;
        return *this;
      }

      /// Strict equality operator for comparing two vectors.
      constexpr bool operator==(const Vector2 v) const {
        return x == v.x && y == v.y;
      }

      /// Strict inequality operator for comparing two vectors.
      constexpr bool operator!=(const Vector2 v) const {
        return x != v.x || y != v.y;
      }

      /// Comparison operator that checks if the square magnitude of this vector is smaller that the square magnitude of vector v
      bool operator<(const Vector2 v) const {
        return SquaredMagnitude() < v.SquaredMagnitude();
      }

      /// Comparison operator that checks if the square magnitude of this vector is greater that the square magnitude of vector v
      bool operator>(const Vector2 v) const {
        return SquaredMagnitude() > v.SquaredMagnitude();
      }

      /// Comparison operator that checks if the square magnitude of this vector is smaller or equal that the square magnitude of vector v
      bool operator<=(const Vector2 v) const {
        return SquaredMagnitude() <= v.SquaredMagnitude();
      }

      /// Comparison operator that checks if the square magnitude of this vector is greater or equal that the square magnitude of vector v
      bool operator>=(const Vector2 v) const {
        return SquaredMagnitude() >= v.SquaredMagnitude();
      }

      /// << operator to allow easy printing of a Vector2 object.
      friend std::ostream &operator<<(std::ostream &os, const Vector2 vec) {
        os << "(" << vec.x << ", " << vec.y << ")";
        return os;
      }

      /// Converts the current vector to a string with this format: (x, y)
      [[nodiscard]] std::string ToString() const {
        return std::to_string(x) + ", " + std::to_string(y);
      }

      /// @returns The magnitude/length/size of this vector
      [[nodiscard]] float Magnitude() const {
        return sqrtf(SquaredMagnitude());
      }

      /// @returns The squared magnitude/length/size of this vector
      [[nodiscard]] constexpr float SquaredMagnitude() const {
        return *this * *this;
      }

      /// @returns The dot product between the current vector and v
      [[nodiscard]] constexpr T Dot(const Vector2 v) const {
        return x * v.x + y * v.y;
      }

      /// @returns The cross product between the current vector and v
      [[nodiscard]] constexpr T Cross(const Vector2 v) const {
        return x * v.y - y * v.x;
      }

      /// @returns The smallest value between the x and y coordinates
      [[nodiscard]] constexpr T Min() const {
        return std::min(x, y);
      }

      /// @returns The biggest value between the x and y coordinates
      [[nodiscard]] constexpr T Max() const {
        return std::max(x, y);
      }

      /// @returns The normalized version of the current vector with a magnitude of 1
      [[nodiscard]] Vector2 Normalized() const {
        if (const float magnitude = Magnitude(); magnitude > 0.0f)
          return *this / magnitude;
        LOG_WARNING(
          "Can not normalize a zero \"(0, 0)\" Vector2, resulting operation will default to not modify the Vector2 instance."
        );
        return *this;
      }

      /**
       * Clamp's the magnitude of the current vector to the given max value
       * @param max The maximum magnitude of this vector
       * @note Calling this method will log a warning if max <= 0
       * @return The clamped version of the current vector
       */
      [[nodiscard]] Vector2 ClampMagnitude(const float max) const {
        if (max <= 0.0f) {
          LOG_WARNING(
            "Parameter max must be greater than zero, resulting operation will default to not modify the Vector2 instance."
          );
          return *this;
        }
        if (const float magnitude = Magnitude(); magnitude > max)
          return *this * (max / magnitude);
        return *this;
      }

      /// @returns The component-wise multiplication
      [[nodiscard]] constexpr Vector2 Scaled(const Vector2 v) const {
        return {x * v.x, y * v.y};
      }

      /// @returns The current vector rotated by the given amount of degrees
      [[nodiscard]] Vector2 Rotated(const float degrees) const {
        return Rotated(degrees, Zero);
      }

      /// @returns The current vector rotated by the given amount of degrees around the given pivot vector
      [[nodiscard]] Vector2 Rotated(const float degrees, const Vector2 &pivot) const {
        const float radians = M_PI * degrees / 180.0f;
        const float s = std::sin(radians);
        const float c = std::cos(radians);
        const float tx = x - pivot.x;
        const float ty = y - pivot.y;

        return {tx * c - ty * s + pivot.x, tx * s + ty * c + pivot.y};
      }

      /// @returns The current vector rotated by the given amount of degrees
      [[nodiscard]] Vector2 RotatedRadians(const float radians) const {
        return RotatedRadians(radians, Zero);
      }

      /// @returns The current vector rotated by the given amount of degrees around the given pivot vector
      [[nodiscard]] Vector2 RotatedRadians(const float radians, const Vector2 pivot) const {
        const float s = std::sin(radians);
        const float c = std::cos(radians);
        const float tx = x - pivot.x;
        const float ty = y - pivot.y;

        return {tx * c - ty * s + pivot.x, tx * s + ty * c + pivot.y};
      }

      /// @returns A vector that is perpendicular to the current vector
      [[nodiscard]] constexpr Vector2 Perpendicular() const {
        return {-y, x};
      }

      /// @returns A vector that is perpendicular to the current vector in a counterclockwise direction
      [[nodiscard]] constexpr Vector2 PerpendicularCounterClockwise() const {
        return {y, -x};
      }

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
      [[nodiscard]] Vector2 MoveTowards(const Vector2 target, float maxDistanceDelta) const {
        if (maxDistanceDelta <= 0.0f)
          return *this;
        const Vector2 direction = target - *this;
        // If the target is closer than maxDistanceDelta, clamp to the target
        if (const float distance = direction.Magnitude(); distance <= maxDistanceDelta || distance == 0.0f)
          return target;
        // Normalize direction and move by maxDistanceDelta
        return *this + direction.Normalized() * maxDistanceDelta;
      }

      /// Normalizes the current vector, giving it a magnitude of 1
      void Normalize() {
        *this = this->Normalized();
      }

      /// Scales the current vector with v by using component-wise multiplication
      constexpr void Scale(const Vector2 v) {
        *this = this->Scaled(v);
      }

      /// Rotates the current vector by the given amount of degrees
      void Rotate(const float degrees) {
        *this = this->Rotated(degrees, Zero);
      }

      /// Rotates the current vector by the given amount of degrees around the given pivot vector
      void Rotate(const float degrees, const Vector2 pivot) {
        *this = this->Rotated(degrees, pivot);
      }

      /// Rotates the current vector by the given amount of degrees
      void RotateRadians(const float radians) {
        *this = this->RotatedRadians(radians, Zero);
      }

      /// Rotates the current vector by the given amount of degrees around the given pivot vector
      void RotateRadians(const float radians, const Vector2 pivot) {
        *this = this->RotatedRadians(radians, pivot);
      }

      /// Converts this Vector2 to a glm::vec2 type for compatibility with OpenGL and GLM libraries.
      [[nodiscard]] constexpr glm::vec2 toGLM() const {
        return glm::vec2{x, y};
      }

      /**
       * Computes the <b>unsigned</b> angle between two vector
       * @param v1 The first vector
       * @param v2 The second vector
       * @return The angle between both vectors
       */
      [[nodiscard]] static float AngleBetween(const Vector2 v1, const Vector2 v2) {
        return std::abs(SignedAngleBetween(v1, v2));
      }

      /**
       * Computes the <b>signed</b> angle between two vector
       * @param v1 The first vector
       * @param v2 The second vector
       * @return The angle between both vectors
       */
      [[nodiscard]] static float SignedAngleBetween(const Vector2 v1, const Vector2 v2) {
        const float v1Mag = v1.Magnitude();
        if (const float v2Mag = v2.Magnitude(); v1Mag > 0.0f && v2Mag > 0.0f)
          return std::acosf(v1 * v2 / (v1Mag * v2Mag));
        if (v1Mag <= 0.0f)
          LOG_WARNING("Parameter v1 can not have be a zero \"(0, 0)\" Vector2, resulting operation will return 0.0f");
        else
          LOG_WARNING("Parameter v2 can not have be a zero \"(0, 0)\" Vector2, resulting operation will return 0.0f");
        return 0.0f;
      }

      /**
       * Computes the squared distance between two vector
       * @param current The first vector
       * @param target The second vector
       * @return The distance separating both vectors squared
       */
      [[nodiscard]] constexpr static float SquaredDistanceTo(const Vector2 current, const Vector2 target) {
        const float dx = current.x - target.x;
        const float dy = current.y - target.y;
        return dx * dx + dy * dy;
      }

      /**
       * Computes the distance between two vector
       * @param current The first vector
       * @param target The second vector
       * @return The distance separating both vectors
       */
      [[nodiscard]] static float DistanceTo(const Vector2 current, const Vector2 target) {
        return std::sqrtf(SquaredDistanceTo(current, target));
      }

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
      [[nodiscard]] constexpr static Vector2 Lerp(const Vector2 current, const Vector2 target, const float t) {
        return LerpUnclamped(current, target, std::clamp(t, 0.0f, 1.0f));
      }

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
      [[nodiscard]] constexpr static Vector2 LerpUnclamped(const Vector2 current, const Vector2 target, float t) {
        return current + (target - current) * t;
      }

      /**
       * Checks if two vector are approximately equal
       * @param v1 The target vector
       * @param v2 The vector to compare to the target
       * @return True if the vectors have a difference in coordinates less than 0.0005f
       */
      [[nodiscard]] static bool ApproxEquals(const Vector2 v1, const Vector2 v2) {
        constexpr float epsilonBase = 0.0005f;
        const float scaledEpsilon = epsilonBase * std::max(1.0f, std::max(std::abs(v1.x), std::abs(v2.x)));
        return SquaredDistanceTo(v1, v2) < scaledEpsilon * scaledEpsilon;
      }
  };

  template<typename T> const Vector2<T> Vector2<T>::Zero = Vector2(T(0), T(0));
  template<typename T> const Vector2<T> Vector2<T>::One = Vector2(T(1), T(1));
  template<typename T> const Vector2<T> Vector2<T>::Right = Vector2(T(1), T(0));
  template<typename T> const Vector2<T> Vector2<T>::Left = Vector2(T(-1), T(0));
  template<typename T> const Vector2<T> Vector2<T>::Up = Vector2(T(0), T(1));
  template<typename T> const Vector2<T> Vector2<T>::Down = Vector2(T(0), T(-1));

  using Vector2f = Vector2<>;
  using Vector2d = Vector2<double>;
  using Vector2Int = Vector2<int>;
}

#endif // VECTOR2_H
