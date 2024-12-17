//
// Entity.cpp
// Author: Antoine Bastide
// Date: 03/11/2024
//

#include <iostream>

#include "2D/Entity2D.h"
#include "2D/Game2D.h"
#include "2D/Physics/Rigidbody2D.h"
#include "Common/Log.h"

namespace Engine2D {
  Entity2D::Entity2D(std::string name, Entity2D *parent, Texture2D *texture)
    : name(std::move(name)), textureColor(glm::vec3(1.0f)), active(true), texture(texture) {
    Game2D::addEntity(this);
    transform.setEntity(this);
    transform.SetParent(parent);
  }

  bool Entity2D::operator==(const Entity2D &entity) const {
    return name == entity.name && transform == entity.transform && texture == entity.texture;
  }

  void Entity2D::AddComponent(Component2D &component) {
    if (typeid(component) == typeid(Transform2D)) {
      LOG_WARNING("Can not add an additional Transform2D to an entity.");
      return;
    }

    component.setEntity(this);
    components.push_back(&component);

    if (const auto rigidbody = dynamic_cast<Physics::Rigidbody2D *>(&component))
      Game2D::instance->physics2D->addRigidBody(rigidbody);
  }

  void Entity2D::RemoveComponent(Component2D &component) {
    if (const auto it = std::ranges::find(components, &component); it != components.end())
      components.erase(it);

    if (const auto rigidbody = dynamic_cast<Physics::Rigidbody2D *>(&component))
      Game2D::instance->physics2D->removeRigidbody(rigidbody);
  }

  void Entity2D::AddComponent(Component2D *component) {
    if (typeid(component) == typeid(Transform2D)) {
      LOG_WARNING("Can not add an additional Transform2D to an entity.");
      return;
    }

    component->setEntity(this);
    components.push_back(component);

    if (const auto rigidbody = dynamic_cast<Physics::Rigidbody2D *>(component))
      Game2D::instance->physics2D->addRigidBody(rigidbody);
  }

  void Entity2D::RemoveComponent(Component2D *component) {
    if (const auto it = std::ranges::find(components, component); it != components.end())
      components.erase(it);

    if (const auto rigidbody = dynamic_cast<Physics::Rigidbody2D *>(component))
      Game2D::instance->physics2D->removeRigidbody(rigidbody);
  }

  void Entity2D::SetTexture(Texture2D *texture) {
    // Do nothing if we are setting the same texture
    if (this->texture == texture)
      return;
    // If the entity already had a texture, remove it from the entities to draw
    if (this->texture) {
      const auto it2 = std::ranges::find_if(
          Game2D::instance->entitiesById[this->texture->id],
          [this](const Entity2D *ptr) {
            return ptr == this;
          }
        );
      if (it2 != Game2D::instance->entitiesById[this->texture->id].end())
        Game2D::instance->entitiesById[this->texture->id].erase(it2);
    }
    // Set the texture and add it to the entities to draw
    this->texture = texture;
    Game2D::instance->entitiesById[this->texture->id].push_back(this);
  }

  Texture2D *Entity2D::Texture() const {
    return texture;
  }

  void Entity2D::Destroy() {
    Game2D::instance->removeEntity(this);
  }

  void Entity2D::initialize() {
    if (!initialized) {
      if (!transform.GetParent())
        this->transform.SetParent(Game2D::instance->root);
      this->Initialize();
      this->initialized = true;
    }
  }

  void Entity2D::update() {
    if (!initialized)
      this->initialize();
    transform.wasUpdated = false;
    this->Update();
  }

  void Entity2D::quit() {
    this->Quit();
    for (const auto component: components)
      RemoveComponent(component);
    components.clear();
    Game2D::removeEntity(this);
  }
}
