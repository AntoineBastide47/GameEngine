//
// Vector2.cpp
// Author: Antoine Bastide
// Date: 03/11/2024
//

#include "2D/Types/Vector2.h"

#include <iostream>
#include <__algorithm/clamp.h>

#include "Common/Log.h"

namespace Engine2D {
  Vector2::Vector2() : x(0.0f), y(0.0f) {}
  Vector2::Vector2(const float x, const float y) : x(x), y(y) {}

  const Vector2 Vector2::Zero = Vector2(0.0f, 0.0f);
  const Vector2 Vector2::One = Vector2(1.0f, 1.0f);
  const Vector2 Vector2::Right = Vector2(1.0f, 0.0f);
  const Vector2 Vector2::Left = Vector2(1.0f, 0.0f);
  const Vector2 Vector2::Up = Vector2(0.0f, 1.0f);
  const Vector2 Vector2::Down = Vector2(-1.0f, 0.0f);

  Vector2 Vector2::operator+(const Vector2 &v) const { return Vector2{x + v.x, y + v.y}; }

  Vector2 &Vector2::operator+=(const Vector2 &v) {
    x += v.x;
    y += v.y;
    return *this;
  }

  Vector2 Vector2::operator-() const { return Vector2{-x, -y}; }
  Vector2 Vector2::operator-(const Vector2 &v) const { return Vector2{x - v.x, y - v.y}; }

  Vector2 &Vector2::operator-=(const Vector2 &v) {
    x -= v.x;
    y -= v.y;
    return *this;
  }

  float Vector2::operator*(const Vector2 &v) const { return x * v.x + y * v.y; }

  Vector2 Vector2::operator*(const float s) const { return Vector2{x * s, y * s}; }

  Vector2 &Vector2::operator*=(const float s) {
    x *= s;
    y *= s;
    return *this;
  }

  Vector2 Vector2::operator/(const float s) const {
    if (s == 0.0f) {
      LOG_WARNING("Can not divide a Vector2 by 0, resulting operation will default to not modify the Vector2 instance.");
      return *this;
    }
    return Vector2{x / s, y / s};
  }

  Vector2 &Vector2::operator/=(const float s) {
    if (s == 0.0f) {
      LOG_WARNING("Can not divide a Vector2 by 0, resulting operation will default to not modify the Vector2 instance.");
      return *this;
    }
    x /= s;
    y /= s;
    return *this;
  }

  float &Vector2::operator[](const int index) {
    if (index == 0)
      return x;
    if (index == 1)
      return y;
    LOG_WARNING("Vector2 index out of range, resulting operation will default to returning the x component of the Vector2 instance.");
    return x;
  }

  bool Vector2::operator==(const Vector2 &v) const { return x == v.x && y == v.y; }
  bool Vector2::operator!=(const Vector2 &v) const { return x != v.x || y != v.y; }
  bool Vector2::operator<(const Vector2 &v) const { return this->SquareMagnitude() < v.SquareMagnitude(); }
  bool Vector2::operator>(const Vector2 &v) const { return this->SquareMagnitude() > v.SquareMagnitude(); }
  bool Vector2::operator<=(const Vector2 &v) const { return this->SquareMagnitude() <= v.SquareMagnitude(); }
  bool Vector2::operator>=(const Vector2 &v) const { return this->SquareMagnitude() >= v.SquareMagnitude(); }

  std::ostream &operator<<(std::ostream &os, const Vector2 &vec) {
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
  }

  std::string Vector2::toString() const { return "(" + std::to_string(x) + ", " + std::to_string(y) + ")"; }

  float Vector2::Magnitude() const { return sqrtf(x * x + y * y); }
  float Vector2::SquareMagnitude() const { return x * x + y * y; }

  Vector2 Vector2::Normalized() const {
    if (const float magnitude = Magnitude(); magnitude > 0.0f)
      return *this / magnitude;
    LOG_WARNING("Can not normalize a zero \"(0, 0)\" Vector2, resulting operation will default to not modify the Vector2 instance.");
    return *this;
  }

  Vector2 Vector2::ClampMagnitude(const float max) const {
    if (max <= 0.0f) {
      LOG_WARNING("Parameter max must be greater than zero, resulting operation will default to not modify the Vector2 instance.");
      return *this;
    }
    if (const float magnitude = Magnitude(); magnitude > max)
      return *this * (max / magnitude);
    return *this;
  }

  float Vector2::Dot(const Vector2 &v) const { return x * v.x + y * v.y; }

  Vector2 Vector2::Scaled(const Vector2 &v) const { return Vector2{x * v.x, y * v.y}; }
  Vector2 Vector2::Perpendicular() const { return Vector2{-y, x}; }

  Vector2 Vector2::MoveTowards(const Vector2 &target, const float maxDistanceDelta) const {
    if (maxDistanceDelta <= 0.0f)
      return *this;
    const Vector2 direction = target - *this;
    // If the target is closer than maxDistanceDelta, clamp to the target
    if (const float distance = direction.Magnitude(); distance <= maxDistanceDelta || distance == 0.0f)
      return target;
    // Normalize direction and move by maxDistanceDelta
    return *this + direction.Normalized() * maxDistanceDelta;
  }

  void Vector2::Normalize() {
    if (const float magnitude = Magnitude(); magnitude > 0.0f)
      *this /= magnitude;
    LOG_WARNING("Can not normalize a zero \"(0, 0)\" Vector2, resulting operation will default to not modify the Vector2 instance.");
  }

  void Vector2::Scale(const Vector2 &v) { *this = this->Scaled(v); }

  glm::vec2 Vector2::toGLM() const { return glm::vec2{x, y}; }

  float Vector2::AngleBetween(const Vector2 &v1, const Vector2 &v2) { return std::abs(SignedAngleBetween(v1, v2)); }

  float Vector2::SignedAngleBetween(const Vector2 &v1, const Vector2 &v2) {
    const float v1Mag = v1.Magnitude();
    if (const float v2Mag = v2.Magnitude(); v1Mag > 0.0f && v2Mag > 0.0f)
      return std::acosf(v1 * v2 / (v1Mag * v2Mag));
    if (v1Mag <= 0.0f)
      LOG_WARNING("Parameter v1 can not have be a zero \"(0, 0)\" Vector2, resulting operation will return 0.0f");
    else
      LOG_WARNING("Parameter v2 can not have be a zero \"(0, 0)\" Vector2, resulting operation will return 0.0f");
    return 0.0f;
  }

  float Vector2::DistanceTo(const Vector2 &current, const Vector2 &target) {
    return std::sqrtf(std::powf(current.x - target.x, 2) + std::powf(current.y - target.y, 2));
  }

  Vector2 Vector2::ClampMagnitude(const Vector2 &target, const float max) {
    if (max <= 0.0f) {
      LOG_WARNING("Parameter max must be greater than zero, resulting operation will default to not modify the Vector2 instance.");
      return target;
    }
    if (const float magnitude = target.Magnitude(); magnitude > max)
      return target * (max / magnitude);
    return target;
  }

  Vector2 Vector2::Lerp(const Vector2 &current, const Vector2 &target, const float t) {
    return LerpUnclamped(current, target, std::clamp(t, 0.0f, 1.0f));
  }

  Vector2 Vector2::LerpUnclamped(const Vector2 &current, const Vector2 &target, const float t) {
    return current + (target - current) * t;
  }

  Vector2 Vector2::MoveTowards(const Vector2 &initial, const Vector2 &target, const float maxDistanceDelta) {
    if (maxDistanceDelta <= 0.0f)
      return initial;
    const Vector2 direction = target - initial;
    // If the target is closer than maxDistanceDelta, clamp to the target
    if (const float distance = direction.Magnitude(); distance <= maxDistanceDelta || distance == 0.0f)
      return target;
    // Normalize direction and move by maxDistanceDelta
    return initial + direction.Normalized() * maxDistanceDelta;
  }
}
