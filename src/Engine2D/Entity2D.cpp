//
// Entity.cpp
// Author: Antoine Bastide
// Date: 03/11/2024
//

#include "Engine2D/Entity2D.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/ParticleSystem/ParticleSystemRegistry2D.hpp"
#include "Engine2D/ParticleSystem/ParticleSystem2D.hpp"
#include "Engine2D/Physics/Physics2D.hpp"
#include "Engine2D/Physics/Collider2D.hpp"
#include "Engine2D/Rendering/Renderer2D.hpp"
#include "Engine2D/Rendering/SpriteRenderer.hpp"
#include "Engine2D/Animation/Animator2D.hpp"
#include "Engine2D/Animation/AnimationSystem.hpp"

namespace Engine2D {
  Entity2D::Entity2D(
    std::string name, const bool isStatic, glm::vec2 position, float rotation, glm::vec2 scale,
    std::shared_ptr<Entity2D> parent
  )
    : name(std::move(name)), transform(std::make_shared<Transform2D>(position, rotation, scale, parent)), active(true),
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
        Rendering::Renderer2D::removeRenderer(renderer);
      else
        Rendering::Renderer2D::addRenderer(renderer);
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
      ParticleSystemRegistry2D::addParticleSystem(particleSystem);
    if (const auto spriteRenderer = std::dynamic_pointer_cast<Rendering::SpriteRenderer>(component))
      Rendering::Renderer2D::addRenderer(spriteRenderer);
    if (const auto animator = std::dynamic_pointer_cast<Animation::Animator2D>(component))
      Animation::AnimationSystem::addAnimator(animator);
  }

  void Entity2D::recallComponent(const std::shared_ptr<Component2D> &component) {
    if (const auto collider = std::dynamic_pointer_cast<Collider2D>(component))
      Game2D::instance->physics2D->removeCollider(collider);
    if (const auto particleSystem = std::dynamic_pointer_cast<ParticleSystem2D>(component))
      ParticleSystemRegistry2D::removeParticleSystem(particleSystem);
    if (const auto spriteRenderer = std::dynamic_pointer_cast<Rendering::SpriteRenderer>(component))
      Rendering::Renderer2D::removeRenderer(spriteRenderer);
    if (const auto animator = std::dynamic_pointer_cast<Animation::Animator2D>(component))
      Animation::AnimationSystem::removeAnimator(animator);
  }
}
