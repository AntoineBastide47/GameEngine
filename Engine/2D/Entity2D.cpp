//
// Entity.cpp
// Author: Antoine Bastide
// Date: 03/11/2024
//

#include "2D/Entity2D.h"
#include "2D/Game2D.h"
#include "Common/Macros.h"

namespace Engine2D {
  Entity2D::Entity2D(std::string name, Entity2D *parent, Texture2D *texture)
    : name(std::move(name)), texture(texture), textureColor(new glm::vec3(1.0f)) {
    Game2D::AddEntity(this); // TODO: Make this work with non pointer constructors
    components.push_back(&transform);
    transform.entity = this;
    transform.SetParent(parent);
  }

  bool Entity2D::operator==(const Entity2D &entity) const {
    return name == entity.name && transform == entity.transform && texture == entity.texture;
  }

  void Entity2D::SetActive(const bool newState) {
    this->active = newState;
  }

  bool Entity2D::IsActive() const {
    return this->active;
  }

  void Entity2D::AddComponent(Component2D &component) {
    components.push_back(&component);
  }

  void Entity2D::RemoveComponent(Component2D &component) {
    if (const auto it = std::ranges::find(components, &component); it != components.end())
      components.erase(it);
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
    SAFE_DELETE(textureColor);
  }
}
