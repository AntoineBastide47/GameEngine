//
// Vector2.h
// Author: Antoine Bastide
// Date: 16/02/2025
//

#ifndef VECTOR2_H
#define VECTOR2_H

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

namespace glm {
  /// Generic function to get the perpendicular vector of a given vector
  template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>> static inline vec<2, T> perpendicular(
    const vec<2, T> &v
  ) {
    return glm::vec<2, T>(-v.y, v.x);
  }

  /// Generic function to get the perpendicular vector of a given vector
  template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>> static inline float cross_2(
    const vec<2, T> &v1, const vec<2, T> &v2
  ) {
    return cross(vec3(v1, 0), vec3(v2, 0)).z;
  }

  template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>> static inline bool approx_equals(
    const vec<2, T> &v1, const vec<2, T> &v2
  ) {
    constexpr float epsilonBase = 0.0005f;
    const float scaledEpsilon = epsilonBase * std::max(1.0f, std::max(std::abs(v1.x), std::abs(v2.x)));
    return distance_squared(v1, v2) < scaledEpsilon * scaledEpsilon;
  }

  template<typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>> static inline float distance_squared(
    const vec<2, T> current, const vec<2, T> target
  ) {
    const vec2 temp = current - target;
    return dot(temp, temp);
  }

  template<typename OStream, length_t L, typename T, qualifier Q> static inline OStream &operator<<(
    OStream &os, const vec<L, T, Q> &vector
  ) {
    return os << glm::to_string(vector);
  }

  static vec2 smoothDamp(
    const vec2 &current, const vec2 &target, vec2 &currentVelocity, const float smoothTime,
    const float deltaTime, const float maxSpeed = std::numeric_limits<float>::infinity()
  ) {
    const float omega = 2.0f / smoothTime;
    const float x = omega * deltaTime;
    const float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

    vec2 change = current - target;
    const float maxChange = maxSpeed * smoothTime;
    const float maxChangeSq = maxChange * maxChange;
    if (const float changeSqLen = dot(change, change); changeSqLen > maxChangeSq)
      change = normalize(change) * maxChange;

    const vec2 temp = (currentVelocity + omega * change) * deltaTime;
    currentVelocity = (currentVelocity - omega * temp) * exp;

    vec2 output = target + (change + temp) * exp;

    // Prevent overshooting
    if (dot(output - target, change) > 0.0f) {
      output = target;
      currentVelocity = vec2(0.0f);
    }

    return output;
  }
} // glm

#endif //VECTOR2_H
