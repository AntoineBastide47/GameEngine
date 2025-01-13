//
// Component2D.cpp
// Author: Antoine Bastide
// Date: 24/11/2024
//

#include "Engine2D/Component2D.h"
#include "Engine2D/Entity2D.h"

namespace Engine2D {
  Component2D::Component2D() : active(true), entity(nullptr) {}

  Component2D::~Component2D() {
    entity = nullptr;
  }

  std::shared_ptr<Entity2D> &Component2D::Entity() {
    return entity;
  }

  Transform2D *Component2D::Transform() const {
    return entity ? &entity->transform : nullptr;
  }

  void Component2D::SetActive(const bool active) {
    this->active = active;
  }

  bool Component2D::IsActive() const {
    return entity->IsActive() && active;
  }

  void Component2D::setEntity(const std::shared_ptr<Entity2D> &entity) {
    if (!entity)
      return;
    this->entity = entity;
  }
}
