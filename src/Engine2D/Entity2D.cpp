//
// Entity.cpp
// Author: Antoine Bastide
// Date: 03/11/2024
//

#include <iostream>

#include "Engine2D/Entity2D.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/ParticleSystem/ParticleSystemRenderer2D.hpp"
#include "Engine2D/ParticleSystem/ParticleSystem2D.hpp"
#include "Engine2D/Physics/Physics2D.hpp"
#include "Engine2D/Rendering/Renderer2D.hpp"
#include "Engine2D/Rendering/SpriteRenderer.hpp"

namespace Engine2D {
  Entity2D::Entity2D(std::string name)
    : name(std::move(name)), transform(std::make_shared<Transform2D>()), active(true), parentsActive(true) {}

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
  }

  bool Entity2D::IsActive() const {
    return this->active && this->parentsActive;
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
  }

  void Entity2D::destroy() {
    for (auto it = components.begin(); it != components.end();) {
      recallComponent(*it);
      it = components.erase(it);
    }
    components.clear();
    transform->RemoveAllChildren();
  }

  void Entity2D::forwardComponent(const std::shared_ptr<Component2D> &component) {
    if (const auto collider = std::dynamic_pointer_cast<Collider2D>(component))
      Game2D::instance->physics2D->addCollider(collider);
    if (const auto particleSystem = std::dynamic_pointer_cast<ParticleSystem2D>(component))
      ParticleSystemRenderer2D::addParticleSystem(particleSystem);
    if (const auto spriteRenderer = std::dynamic_pointer_cast<Rendering::SpriteRenderer>(component))
      Rendering::Renderer2D::addRenderer(spriteRenderer);
  }

  void Entity2D::recallComponent(const std::shared_ptr<Component2D> &component) {
    if (const auto collider = std::dynamic_pointer_cast<Collider2D>(component))
      Game2D::instance->physics2D->removeCollider(collider);
    if (const auto particleSystem = std::dynamic_pointer_cast<ParticleSystem2D>(component))
      ParticleSystemRenderer2D::removeParticleSystem(particleSystem);
    if (const auto spriteRenderer = std::dynamic_pointer_cast<Rendering::SpriteRenderer>(component))
      Rendering::Renderer2D::removeRenderer(spriteRenderer);
  }
}
