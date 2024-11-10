//
// Vector2.cpp
// Implementation for Vector2
// Author: Antoine Bastide
// Date: 03/11/2024
//

#include "2D/Types/Vector2.h"

namespace Engine2D {
  Vector2::Vector2() : x(0.0f), y(0.0f) {}
  Vector2::Vector2(const float x, const float y) : x(x), y(y) {}

  const Vector2 Vector2::Zero = Vector2(0.0f, 0.0f);
  const Vector2 Vector2::One = Vector2(1.0f, 1.0f);
  const Vector2 Vector2::Right = Vector2(1.0f, 0.0f);
  const Vector2 Vector2::Left = Vector2(1.0f, 0.0f);
  const Vector2 Vector2::Up = Vector2(0.0f, 1.0f);
  const Vector2 Vector2::Down = Vector2(-1.0f, 0.0f);

  Vector2 Vector2::operator-() const { return Vector2{-x, -y}; }
  Vector2 Vector2::operator+(const Vector2 &v) const { return Vector2{x + v.x, y + v.y}; }
  Vector2 Vector2::operator-(const Vector2 &v) const { return Vector2{x - v.x, y - v.y}; }
  Vector2 Vector2::operator*(const Vector2 &v) const { return Vector2{x * v.x, y * v.y}; }
  Vector2 Vector2::operator/(const Vector2 &v) const { return Vector2{x / v.x, y / v.y}; }

  Vector2 &Vector2::operator+=(const Vector2 &v) {
    x += v.x;
    y += v.y;
    return *this;
  }

  Vector2 &Vector2::operator-=(const Vector2 &v) {
    x -= v.x;
    y -= v.y;
    return *this;
  }

  Vector2 &Vector2::operator*=(const Vector2 &v) {
    x *= v.x;
    y *= v.y;
    return *this;
  }

  Vector2 &Vector2::operator/=(const Vector2 &v) {
    x /= v.x;
    y /= v.y;
    return *this;
  }

  Vector2 Vector2::operator*(const float s) const { return Vector2{x * s, y * s}; }
  Vector2 Vector2::operator/(const float s) const { return Vector2{x / s, y / s}; }

  bool Vector2::operator==(const Vector2 &v) const { return (x == v.x && y == v.y); }
  bool Vector2::operator!=(const Vector2 &v) const { return !(*this == v); }

  glm::vec2 Vector2::toGLM() const { return glm::vec2{x, y}; }
}
