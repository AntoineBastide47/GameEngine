//
// Vector2.cpp
// Author: Antoine Bastide
// Date: 03/11/2024
//

#include <algorithm>
#include <iostream>

#include "2D/Types/Vector2.h"
#include "Common/Log.h"

namespace Engine2D {
  Vector2::Vector2() : x(0.0f), y(0.0f) {}
  Vector2::Vector2(const float x, const float y) : x(x), y(y) {}

  const Vector2 Vector2::Zero = Vector2(0.0f, 0.0f);
  const Vector2 Vector2::One = Vector2(1.0f, 1.0f);
  const Vector2 Vector2::Right = Vector2(1.0f, 0.0f);
  const Vector2 Vector2::Left = Vector2(-1.0f, 0.0f);
  const Vector2 Vector2::Up = Vector2(0.0f, 1.0f);
  const Vector2 Vector2::Down = Vector2(0.0f, -1.0f);

  Vector2 Vector2::operator+(const Vector2 &v) const {
    return Vector2{x + v.x, y + v.y};
  }

  Vector2 &Vector2::operator+=(const Vector2 &v) {
    return *this = *this + v;
  }

  Vector2 Vector2::operator-() const {
    return Vector2{-x, -y};
  }

  Vector2 Vector2::operator-(const Vector2 &v) const {
    return Vector2{x - v.x, y - v.y};
  }

  Vector2 &Vector2::operator-=(const Vector2 &v) {
    return *this = *this - v;
  }

  float Vector2::operator*(const Vector2 &v) const {
    return x * v.x + y * v.y;
  }

  Vector2 Vector2::operator*(const float scalar) const {
    return Vector2{x * scalar, y * scalar};
  }

  Vector2 &Vector2::operator*=(const float scalar) {
    return *this = *this * scalar;
  }

  Vector2 Vector2::operator%(const float scalar) const {
    return Vector2{std::fmod(x, scalar), std::fmod(y, scalar)};
  }

  Vector2 &Vector2::operator%=(const float scalar) {
    return *this = *this % scalar;
  }

  float Vector2::operator^(const Vector2 &v) const {
    return x * v.y - y * v.x;
  }

  Vector2 Vector2::operator/(const float scalar) const {
    if (scalar == 0.0f) {
      LOG_WARNING("Can not divide a Vector2 by 0, resulting operation will default to not modify the Vector2 instance.");
      return *this;
    }
    return *this * (1.0f / scalar);
  }

  Vector2 &Vector2::operator/=(const float scalar) {
    if (scalar == 0.0f) {
      LOG_WARNING("Can not divide a Vector2 by 0, resulting operation will default to not modify the Vector2 instance.");
      return *this;
    }
    return *this = *this / scalar;
  }

  float &Vector2::operator[](const int index) {
    if (index == 0)
      return x;
    if (index == 1)
      return y;
    LOG_WARNING(
      "Vector2 index out of range, resulting operation will default to returning the x component of the Vector2 instance."
    );
    return x;
  }

  Vector2 &Vector2::operator=(const glm::vec2 &other) {
    x = other.x;
    y = other.y;
    return *this;
  }

  bool Vector2::operator==(const Vector2 &v) const {
    return x == v.x && y == v.y;
  }

  bool Vector2::operator!=(const Vector2 &v) const {
    return !(*this == v);
  }

  bool Vector2::operator<(const Vector2 &v) const {
    return this->SquaredMagnitude() < v.SquaredMagnitude();
  }

  bool Vector2::operator>(const Vector2 &v) const {
    return this->SquaredMagnitude() > v.SquaredMagnitude();
  }

  bool Vector2::operator<=(const Vector2 &v) const {
    return this->SquaredMagnitude() <= v.SquaredMagnitude();
  }

  bool Vector2::operator>=(const Vector2 &v) const {
    return this->SquaredMagnitude() >= v.SquaredMagnitude();
  }

  Vector2 operator*(const float scalar, const Vector2 &vec) {
    return vec * scalar;
  }

  Vector2 operator%(const float scalar, const Vector2 &vec) {
    return vec % scalar;
  }

  Vector2 operator/(const float scalar, const Vector2 &vec) {
    return vec / scalar;
  }

  std::ostream &operator<<(std::ostream &os, const Vector2 &vec) {
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
  }

  std::string Vector2::ToString() const {
    return "(" + std::to_string(x) + ", " + std::to_string(y) + ")";
  }

  float Vector2::Magnitude() const {
    return sqrtf(SquaredMagnitude());
  }

  float Vector2::SquaredMagnitude() const {
    return *this * *this;
  }

  Vector2 Vector2::Normalized() const {
    if (const float magnitude = Magnitude(); magnitude > 0.0f)
      return *this / magnitude;
    LOG_WARNING(
      "Can not normalize a zero \"(0, 0)\" Vector2, resulting operation will default to not modify the Vector2 instance."
    );
    return *this;
  }

  Vector2 Vector2::ClampMagnitude(const float max) const {
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

  float Vector2::Dot(const Vector2 &v) const {
    return x * v.x + y * v.y;
  }

  float Vector2::Cross(const Vector2 &v) const {
    return x * v.y - y * v.x;
  }

  float Vector2::Min() const {
    return x < y ? x : y;
  }

  float Vector2::Max() const {
    return x > y ? x : y;
  }

  Vector2 Vector2::Rotated(const float degrees) const {
    return Rotated(degrees, Zero);
  }

  Vector2 Vector2::Rotated(const float degrees, const Vector2 &pivot) const {
    const float radians = M_PI * degrees / 180.0f;
    const float s = std::sin(radians);
    const float c = std::cos(radians);
    const float tx = x - pivot.x;
    const float ty = y - pivot.y;

    return Vector2{tx * c - ty * s + pivot.x, tx * s + ty * c + pivot.y};
  }

  Vector2 Vector2::Scaled(const Vector2 &v) const {
    return Vector2{x * v.x, y * v.y};
  }

  Vector2 Vector2::Perpendicular() const {
    return Vector2{-y, x};
  }

  Vector2 Vector2::PerpendicularCounterClockwise() const {
    return Vector2{y, -x};
  }

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
    LOG_WARNING(
      "Can not normalize a zero \"(0, 0)\" Vector2, resulting operation will default to not modify the Vector2 instance."
    );
  }

  void Vector2::Scale(const Vector2 &v) {
    *this = this->Scaled(v);
  }

  void Vector2::Rotate(const float degrees) {
    *this = this->Rotated(degrees, Zero);
  }

  void Vector2::Rotate(const float degrees, const Vector2 &pivot) {
    *this = this->Rotated(degrees, pivot);
  }

  glm::vec2 Vector2::toGLM() const {
    return glm::vec2{x, y};
  }

  float Vector2::AngleBetween(const Vector2 &v1, const Vector2 &v2) {
    return std::abs(SignedAngleBetween(v1, v2));
  }

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

  float Vector2::SquaredDistanceTo(const Vector2 &current, const Vector2 &target) {
    const float dx = current.x - target.x;
    const float dy = current.y - target.y;
    return dx * dx + dy * dy;
  }

  float Vector2::DistanceTo(const Vector2 &current, const Vector2 &target) {
    return std::sqrtf(SquaredDistanceTo(current, target));
  }

  Vector2 Vector2::Lerp(const Vector2 &current, const Vector2 &target, const float t) {
    return LerpUnclamped(current, target, std::clamp(t, 0.0f, 1.0f));
  }

  Vector2 Vector2::LerpUnclamped(const Vector2 &current, const Vector2 &target, const float t) {
    return current + (target - current) * t;
  }

  bool Vector2::ApproxEquals(const Vector2 &v1, const Vector2 &v2) {
    constexpr float epsilonBase = 0.0005f;
    const float scaledEpsilon = epsilonBase * std::max(1.0f, std::max(std::abs(v1.x), std::abs(v2.x)));
    return SquaredDistanceTo(v1, v2) < scaledEpsilon * scaledEpsilon;
  }
}
