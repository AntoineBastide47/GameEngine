//
// Entity.cpp
// Author: Antoine Bastide
// Date: 03/11/2024
//

#include <ranges>

#include "Engine2D/Entity2D.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/Rendering/SpriteRenderer.hpp"

namespace Engine2D {
  Entity2D::Entity2D()
    : Entity2D("Entity", false, {0, 0}, 0, {0, 0}, nullptr) {}

  Entity2D::Entity2D(
    std::string name, const bool isStatic, const glm::vec2 position, const float rotation, const glm::vec2 scale,
    Entity2D *parent
  )
    : name(std::move(name)), active(true), parentsActive(true), isStatic(isStatic), destroyed(false),
      framesSinceDestroyed(0),
      transform(std::unique_ptr<Transform2D>(new Transform2D(position, rotation, scale, this, parent))) {}

  bool Entity2D::operator==(const Entity2D &entity) const {
    return name == entity.name && transform == entity.transform;
  }

  bool Entity2D::operator!=(const Entity2D &entity) const {
    return !(*this == entity);
  }

  Transform2D *Entity2D::Transform() const {
    return transform.get();
  }

  void Entity2D::SetActive(const bool active) {
    this->active = active;
    for (const auto child: *transform)
      child->transform->onParentHierarchyChange();

    // All change the state of the renderer
    if (const auto renderer = GetComponent<Rendering::SpriteRenderer>(); renderer) {
      if (!active)
        renderer->recall();
      else
        renderer->forward();
    }
  }

  bool Entity2D::IsActive() const {
    return !destroyed && active && parentsActive;
  }

  bool Entity2D::IsStatic() const {
    return isStatic;
  }

  void Entity2D::Destroy() {
    Game2D::instance->removeEntity(this);
  }

  void Entity2D::initialize() const {
    transform->onTransformChange();
    transform->onParentHierarchyChange();
    if (!transform->parent)
      transform->SetParent(nullptr);
  }

  void Entity2D::destroy() {
    for (auto it = components.begin(); it != components.end(); ++it)
      (*it)->recall();
    transform->RemoveAllChildren();
    SetActive(false);
    destroyed = true;
  }

  void Entity2D::free() {
    components.clear();
    behaviours.clear();
  }
}
