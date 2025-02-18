//
// Vector2.h
// Author: Antoine Bastide
// Date: 16/02/2025
//

#ifndef VECTOR2_H
#define VECTOR2_H

#include <glm/vec2.hpp>
#include <glm/glm.hpp>

namespace glm {
  /// Generic function to rotate a 2D vector for any arithmetic type
  template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>> glm::vec<2, T> rotate_vector2(
    const glm::vec<2, T> &v, T radians
  ) {
    T cosTheta = glm::cos(radians);
    T sinTheta = glm::sin(radians);

    return glm::vec<2, T>(
      v.x * cosTheta - v.y * sinTheta,
      v.x * sinTheta + v.y * cosTheta
    );
  }

  /// Wrapper to rotate using degrees instead of radians
  template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>> glm::vec<2, T> rotate_vector2_degrees(
    const glm::vec<2, T> &v, T degrees
  ) {
    return rotate_vector2(v, glm::radians(degrees));
  }

  /// Generic function to get the perpendicular vector of a given vector
  template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>> glm::vec<2, T> perpendicular(
    const glm::vec<2, T> &v
  ) {
    return glm::vec<2, T>(-v.y, v.x);
  }

  /// Generic function to get the perpendicular vector of a given vector
  template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>> float cross_2(
    const glm::vec<2, T> &v1, const glm::vec<2, T> &v2
  ) {
    return glm::cross(glm::vec3(v1, 0), glm::vec3(v2, 0)).z;
  }

  template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>> bool approx_equals(
    const glm::vec<2, T> &v1, const glm::vec<2, T> &v2
  ) {
    constexpr float epsilonBase = 0.0005f;
    const float scaledEpsilon = epsilonBase * std::max(1.0f, std::max(std::abs(v1.x), std::abs(v2.x)));
    return distance_squared(v1, v2) < scaledEpsilon * scaledEpsilon;
  }

  template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>> float length_squared(
    const glm::vec<2, T> &v
  ) {
    const float length = glm::length(v);
    return length * length;
  }

  template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>> static constexpr float distance_squared(
    const glm::vec<2, T> current, const glm::vec<2, T> target
  ) {
    const float d = glm::distance(current, target);
    return d * d;
  }
} // glm

#endif //VECTOR2_H
