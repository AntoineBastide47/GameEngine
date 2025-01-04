//
// Entity.cpp
// Author: Antoine Bastide
// Date: 03/11/2024
//

#include <iostream>

#include "2D/Entity2D.h"
#include "2D/Game2D.h"
#include "2D/Physics/Physics2D.h"
#include "2D/Physics/Rigidbody2D.h"

using Engine2D::Rendering::Texture2D;

namespace Engine2D {
  Entity2D::Entity2D(std::string name)
    : name(std::move(name)), textureColor(glm::vec3(1.0f)), initialized(false), active(true), parentsActive(true), texture(nullptr) {}

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
      Game2D::instance->entitiesToRender[this->texture->id].erase(shared_from_this());
    this->texture = texture;
    Game2D::instance->entitiesToRender[this->texture->id].insert(shared_from_this());
  }

  std::shared_ptr<Texture2D> Entity2D::Texture() const {
    return texture;
  }

  void Entity2D::Destroy() {
    Game2D::instance->removeEntity(shared_from_this());
  }

  void Entity2D::initialize() {
    if (!initialized) {
      if (!weak_from_this().lock())
        throw std::runtime_error(name + " must be created using Game2D::AddEntity.");

      transform.setEntity(shared_from_this());
      transform.onParentHierarchyChange();
      this->Initialize();
      if (!this->transform.parent)
        this->transform.SetParent(nullptr);
      this->initialized = true;
    }
  }

  void Entity2D::update() {
    if (!initialized)
      this->initialize();
    this->Update();
  }

  void Entity2D::destroy() {
    this->OnDestroy();
    for (auto it = components.begin(); it != components.end();) {
      const auto component = *it;
      if (const auto rigidbody = std::dynamic_pointer_cast<Rigidbody2D>(component))
        Game2D::instance->physics2D->removeRigidbody(rigidbody);
      it = components.erase(it);
    }
    components.clear();
  }

  void Entity2D::forwardComponent(const std::shared_ptr<Component2D> &component) {
    if (const auto rigidbody = std::dynamic_pointer_cast<Rigidbody2D>(component))
      Game2D::instance->physics2D->addRigidBody(rigidbody);
    if (const auto collider = std::dynamic_pointer_cast<Collider2D>(component))
      Game2D::instance->physics2D->addCollider(collider);
  }

  void Entity2D::recallComponent(const std::shared_ptr<Component2D> &component) {
    if (const auto rigidbody = std::dynamic_pointer_cast<Rigidbody2D>(component))
      Game2D::instance->physics2D->removeRigidbody(rigidbody);
    if (const auto collider = std::dynamic_pointer_cast<Collider2D>(component))
      Game2D::instance->physics2D->removeCollider(collider);
  }
}
