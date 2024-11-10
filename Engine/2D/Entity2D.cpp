//
// Entity.cpp
// Entity2D: Represents a 2D entity with transform, rendering, and hierarchical capabilities
// Author: Antoine Bastide
// Date: 03/11/2024
//

#include "2D/Game2D.h"
#include "2D/Entity2D.h"

namespace Engine2D {
  Entity2D::Entity2D(std::string name, Entity2D *parent, const Rendering::Texture2D& texture)
    : name(std::move(name)), parent(parent), texture(texture) { Game2D::AddEntity(this); }

  bool Entity2D::operator==(const Entity2D &entity) const { return name == entity.name && parent == entity.parent; }

  void Entity2D::initialize() {
    if (!parent)
      this->SetParent(Game2D::Instance()->mainParent);
    this->Initialize();
  }

  void Entity2D::SetParent(Entity2D *parent) { this->parent = parent; }

  void Entity2D::quit() {
    if (parent) {
      delete parent;
      parent = nullptr;
    }
    Game2D::RemoveEntity(this);
    this->Quit();
  }
}
