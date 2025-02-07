//
// Entity.cpp
// Author: Antoine Bastide
// Date: 03/11/2024
//

#include "Engine2D/Entity2D.h"
#include "Engine2D/Game2D.h"
#include "Engine2D/ParticleSystem/ParticleSystemRenderer2D.h"
#include "Engine2D/ParticleSystem/ParticleSystem2D.h"
#include "Engine2D/Physics/Physics2D.h"

using Engine2D::Rendering::Texture2D;

namespace Engine2D {
  Entity2D::Entity2D(std::string name)
    : name(std::move(name)), textureColor(glm::vec4(1.0f)), active(true), parentsActive(true), texture(nullptr) {}

  bool Entity2D::operator==(const Entity2D &entity) const {
    return name == entity.name && transform == entity.transform && texture == entity.texture;
  }

  bool Entity2D::operator!=(const Entity2D &entity) const {
    return !(*this == entity);
  }

  void Entity2D::SetActive(const bool active) {
    this->active = active;
    for (const auto child: transform)
      child->transform.onParentHierarchyChange();
  }

  bool Entity2D::IsActive() const {
    return this->active && this->parentsActive;
  }

  void Entity2D::SetTexture(const std::shared_ptr<Texture2D> &texture) {
    if (!texture || this->texture == texture)
      return;

    if (this->texture)
      for (const auto it = Game2D::instance->entitiesToRender[this->texture->id].begin();
           it != Game2D::instance->entitiesToRender[this->texture->id].end();)
        if (*it == shared_from_this())
          Game2D::instance->entitiesToRender[this->texture->id].erase(it);
    this->texture = texture;
    Game2D::instance->entitiesToRender[this->texture->id].emplace_back(shared_from_this());
  }

  std::shared_ptr<Texture2D> Entity2D::Texture() const {
    return texture;
  }

  void Entity2D::Destroy() {
    Game2D::instance->removeEntity(shared_from_this());
  }

  void Entity2D::initialize() {
    if (!weak_from_this().lock())
      throw std::runtime_error(name + " must be created using Game2D::AddEntity.");

    transform.setEntity(shared_from_this());
    transform.onParentHierarchyChange();
    this->OnInitialize();
    if (!this->transform.parent)
      this->transform.SetParent(nullptr);
  }

  void Entity2D::destroy() {
    this->OnDestroy();
    for (auto it = components.begin(); it != components.end();) {
      recallComponent(*it);
      it = components.erase(it);
    }
    components.clear();
  }

  void Entity2D::forwardComponent(const std::shared_ptr<Component2D> &component) {
    if (const auto collider = std::dynamic_pointer_cast<Collider2D>(component))
      Game2D::instance->physics2D->addCollider(collider);
    if (const auto particleSystem = std::dynamic_pointer_cast<ParticleSystem2D>(component))
      ParticleSystemRenderer2D::addParticleSystem(particleSystem);

    component->OnInitialize();
  }

  void Entity2D::recallComponent(const std::shared_ptr<Component2D> &component) {
    if (const auto collider = std::dynamic_pointer_cast<Collider2D>(component))
      Game2D::instance->physics2D->removeCollider(collider);
    if (const auto particleSystem = std::dynamic_pointer_cast<ParticleSystem2D>(component))
      ParticleSystemRenderer2D::removeParticleSystem(particleSystem);

    component->OnDestroy();
  }
}
