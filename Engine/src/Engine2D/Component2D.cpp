//
// Component2D.cpp
// Author: Antoine Bastide
// Date: 24/11/2024
//

#include "Engine2D/Component2D.hpp"

#include <__ostream/basic_ostream.h>

#include "Engine2D/Entity2D.hpp"

namespace Engine2D {
  Component2D::Component2D()
    : active(true), entity(nullptr) {}

  Entity2D *Component2D::Entity() const {
    return entity;
  }

  Transform2D *Component2D::Transform() const {
    return entity ? entity->Transform() : nullptr;
  }

  void Component2D::SetActive(const bool active) {
    this->active = active;
  }

  bool Component2D::IsActive() const {
    return entity->IsActive() && active;
  }

  std::ostream &operator<<(std::ostream &os, const Component2D &component) {
    os.write(component.ClassNameQualified().data(), component.ClassNameQualified().size());
    if (component.Entity()) {
      os.put(' ');
      os.put('(');
      os.write(component.Entity()->name.c_str(), component.Entity()->name.size());
      os.put(')');
    }
    os.put('\n');
    return os;
  }
}
