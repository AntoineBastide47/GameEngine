//
// Entity.cpp
// Author: Antoine Bastide
// Date: 03/11/2024
//

#include "Engine2D/Entity2D.hpp"
#include "Engine2D/Behaviour.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/Rendering/SpriteRenderer.hpp"

namespace Engine2D {
  Entity2D::Entity2D()
    : Entity2D("Entity", false, {0, 0}, 0, {0, 0}, nullptr) {}

  Entity2D::Entity2D(
    std::string name, const bool isStatic, const glm::vec2 position, const float rotation, const glm::vec2 scale,
    const std::shared_ptr<Entity2D> &parent
  )
    : name(std::move(name)),
      transform(std::shared_ptr<Transform2D>(new Transform2D(position, rotation, scale, parent))), active(true),
      parentsActive(true), isStatic(isStatic) {}

  bool Entity2D::operator==(const Entity2D &entity) const {
    return name == entity.name && transform == entity.transform;
  }

  bool Entity2D::operator!=(const Entity2D &entity) const {
    return !(*this == entity);
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
    return this->active && this->parentsActive;
  }

  bool Entity2D::IsStatic() const {
    return this->isStatic;
  }

  void Entity2D::Destroy() {
    Game2D::instance->removeEntity(shared_from_this());
  }

  void Entity2D::initialize() {
    if (!weak_from_this().lock())
      throw std::runtime_error(name + " must be created using Game2D::AddEntity.");

    transform->setEntity(shared_from_this());
    transform->onParentHierarchyChange();
    if (!transform->parent)
      transform->SetParent(nullptr);
    transform->initialized = true;
  }

  void Entity2D::destroy() {
    for (auto it = components.begin(); it != components.end();) {
      it->get()->recall();
      it = components.erase(it);
    }
    behaviours.clear();
    transform->RemoveAllChildren();
  }
}
