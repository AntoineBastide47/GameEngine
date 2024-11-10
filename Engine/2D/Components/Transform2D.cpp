//
// Transform2D.cpp
// Transform2D: 
// Author: Antoine Bastide
// Date: 03/11/2024
//

#include "2D/Components/Transform2D.h"

namespace Engine2D {
  Transform2D::Transform2D() : position(Vector2::Zero), rotation(0.0f), scale(Vector2::One) {}

  Transform2D::Transform2D(const Vector2 &position, const float &rotation, const Vector2 &scale)
    : position(position), rotation(rotation), scale(scale) {}

  bool Transform2D::operator==(const Transform2D &other) const {
    return position == other.position && rotation == other.rotation && scale == other.scale;
  }

  bool Transform2D::operator!=(const Transform2D &transform) const { return !(*this == transform); }
}
