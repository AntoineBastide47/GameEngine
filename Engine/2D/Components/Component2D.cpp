//
// Component2D.cpp
// Author: Antoine Bastide
// Date: 24/11/2024
//

#include "2D/Components/Component2D.h"
#include "2D/Entity2D.h"

namespace Engine2D {
  std::shared_ptr<Entity2D> &Component2D::Entity() {
    return entity;
  }

  Transform2D *Component2D::Transform() const {
    return transform;
  }

  Component2D::Component2D() : active(true), entity(nullptr), transform(nullptr) {}

  Component2D::~Component2D() {
    transform = nullptr;
    entity = nullptr;
  }

  void Component2D::setEntity(const std::shared_ptr<Entity2D> &entity) {
    if (!entity)
      return;
    this->entity = entity;
    this->transform = &entity->transform;
  }
}
