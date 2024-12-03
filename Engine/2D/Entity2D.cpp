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
    : name(std::move(name)), texture(texture), textureColor(glm::vec3(1.0f)) {
    Game2D::AddEntity(this); // TODO: Make this work with non pointer constructors
    transform.SetEntity(this);
    transform.SetParent(parent);
  }

  bool Entity2D::operator==(const Entity2D &entity) const {
    return name == entity.name && transform == entity.transform && texture == entity.texture;
  }

  void Entity2D::SetActive(const bool newState) {
    this->active = newState;
    for (const auto child: transform)
      if (child)
        child->active = newState;
  }

  bool Entity2D::IsActive() const {
    return this->active;
  }

  void Entity2D::AddComponent(Component2D &component) {
    if (typeid(component) == typeid(Transform2D)) {
      LOG_WARNING("Can not add an additional Transform2D to an entity.");
      return;
    }

    component.SetEntity(this);
    components.push_back(&component);

    if (const auto rigidbody = dynamic_cast<Physics::Rigidbody2D *>(&component))
      Game2D::Instance()->physics2D->addRigidBody(rigidbody);
  }

  void Entity2D::RemoveComponent(Component2D &component) {
    if (const auto it = std::ranges::find(components, &component); it != components.end())
      components.erase(it);

    if (const auto rigidbody = dynamic_cast<Physics::Rigidbody2D *>(&component))
      Game2D::Instance()->physics2D->removeRigidbody(rigidbody);
  }

  void Entity2D::Destroy() {
    quit();
  }

  void Entity2D::initialize() {
    if (!initialized) {
      if (!transform.GetParent())
        this->transform.SetParent(Game2D::Instance()->root);
      this->Initialize();
      this->initialized = true;
    }
  }

  void Entity2D::update() {
    if (!initialized)
      this->initialize();
    this->Update();
  }

  void Entity2D::quit() {
    this->Quit();
    for (const auto component: components)
      RemoveComponent(*component);
    components.clear();
    Game2D::RemoveEntity(this);
  }
}
