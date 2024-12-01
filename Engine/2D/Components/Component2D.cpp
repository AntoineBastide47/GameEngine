//
// Component2D.cpp
// Author: Antoine Bastide
// Date: 24/11/2024
//

#include "2D/Components/Component2D.h"
#include "2D/Entity2D.h"

namespace Engine2D {
  Entity2D *Component2D::Entity() const {
    return entity;
  }

  Transform2D * Component2D::Transform() const {
    return transform;
  }

  Component2D::~Component2D() {
    transform = nullptr;
    entity = nullptr;
  }

  void Component2D::SetEntity(Entity2D *entity) {
    if (!entity)
      return;
    this->entity = entity;
    this->transform = &entity->transform;
  }
}
