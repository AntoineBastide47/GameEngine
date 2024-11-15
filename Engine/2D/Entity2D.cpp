//
// Entity.cpp
// Author: Antoine Bastide
// Date: 03/11/2024
//

#include "2D/Game2D.h"
#include "2D/Entity2D.h"

namespace Engine2D {
  Entity2D::Entity2D(std::string name, Entity2D *parent, Texture2D *texture)
    : name(std::move(name)), parent(parent), texture(texture), initialized(false) { Game2D::AddEntity(this); }

  bool Entity2D::operator==(const Entity2D &entity) const {
    return name == entity.name && parent == entity.parent && texture == entity.texture;
  }

  void Entity2D::initialize() {
    if (!parent)
      this->SetParent(Game2D::Instance()->root);
    this->Initialize();
    this->initialized = true;
  }

  void Entity2D::SetParent(Entity2D *parent) { this->parent = parent; }

  void Entity2D::quit() {
    this->Quit();
    if (parent) {
      delete parent;
      parent = nullptr;
    }
  }
}
