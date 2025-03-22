//
// Vector2.h
// Author: Antoine Bastide
// Date: 16/02/2025
//

#ifndef VECTOR2_H
#define VECTOR2_H

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
#include <glm/glm.hpp>

namespace glm {
  /// Generic function to get the perpendicular vector of a given vector
  template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>> inline glm::vec<2, T> perpendicular(
    const glm::vec<2, T> &v
  ) {
    return glm::vec<2, T>(-v.y, v.x);
  }

  /// Generic function to get the perpendicular vector of a given vector
  template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>> inline float cross_2(
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

  template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>> static inline float distance_squared(
    const glm::vec<2, T> current, const glm::vec<2, T> target
  ) {
    const vec2 temp = current - target;
    return glm::dot(temp, temp);
  }

  template<typename OStream, glm::length_t L, typename T, glm::qualifier Q> inline OStream &operator<<(
    OStream &os, const glm::vec<L, T, Q> &vector
  ) {
    return os << glm::to_string(vector);
  }
} // glm

#endif //VECTOR2_H
