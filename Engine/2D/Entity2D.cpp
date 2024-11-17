//
// Entity.cpp
// Author: Antoine Bastide
// Date: 03/11/2024
//

#include "2D/Game2D.h"
#include "2D/Entity2D.h"

#include "Common/Macros.h"

namespace Engine2D {
  Entity2D::Entity2D(std::string name, Entity2D *parent, Texture2D *texture)
    : name(std::move(name)), parent(parent), texture(texture), textureColor(new glm::vec3(1.0f)) {
    // TODO: Make this work with non pointer constructors
    Game2D::AddEntity(this);
  }

  bool Entity2D::operator==(const Entity2D &entity) const {
    return name == entity.name && parent == entity.parent && texture == entity.texture;
  }

  void Entity2D::SetParent(Entity2D *parent) {
    this->parent = parent;
  }

  void Entity2D::SetActive(const bool newState) {
    active = newState;
  }

  bool Entity2D::IsActive() const {
    return this->active;
  }

  void Entity2D::initialize() {
    if (!initialized) {
      if (!parent)
        this->SetParent(Game2D::Instance()->root);
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
    SAFE_DELETE(parent);
    SAFE_DELETE(textureColor);
  }
}
