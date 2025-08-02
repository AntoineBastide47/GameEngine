//
// Entity.cpp
// Author: Antoine Bastide
// Date: 03/11/2024
//

#include <ranges>

#include "Engine2D/Entity2D.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/SceneManagement/SceneManager.hpp"
#include "Engine2D/Rendering/SpriteRenderer.hpp"

namespace Engine2D {
  Entity2D::Entity2D()
    : Entity2D("Entity", false, {0, 0}, 0, {0, 0}, nullptr) {}

  Entity2D::Entity2D(
    std::string name, const bool isStatic, const glm::vec2 position, const float rotation, const glm::vec2 scale,
    Entity2D *parent
  )
    : name(std::move(name)), active(true), parentsActive(true), isStatic(isStatic), destroyed(false),
      timeToLive(0.1f),
      transform(std::unique_ptr<Transform2D>(new Transform2D(position, rotation, scale, this, parent))),
      scene(nullptr) {}

  bool Entity2D::operator==(const Entity2D &entity) const {
    return name == entity.name && transform == entity.transform;
  }

  bool Entity2D::operator!=(const Entity2D &entity) const {
    return !(*this == entity);
  }

  Transform2D *Entity2D::Transform() const {
    return transform.get();
  }

  Entity2D *Entity2D::Instantiate(
    const std::string &name, const bool isStatic, const glm::vec2 position, const float rotation, const glm::vec2 scale,
    Entity2D *parent
  ) {
    const auto entity = new Entity2D(name, isStatic, position, rotation, scale, parent);
    SceneManager::ActiveScene()->addEntity(std::unique_ptr<Entity2D>(entity));
    return entity;
  }

  void Entity2D::SetActive(const bool active) {
    this->active = active;
    for (const auto child: *transform)
      child->transform->onParentHierarchyChange();
  }

  bool Entity2D::IsActive() const {
    return !destroyed && active && parentsActive;
  }

  bool Entity2D::IsStatic() const {
    return isStatic;
  }

  void Entity2D::Destroy() {
    if (SceneManager::ActiveScene())
      SceneManager::ActiveScene()->removeEntity(this);
  }

  Scene *Entity2D::Scene() const {
    return scene;
  }

  void Entity2D::initialize() const {
    transform->onTransformChange();
    transform->onParentHierarchyChange();
    if (!transform->parent)
      transform->SetParent(nullptr);
  }

  void Entity2D::destroy() {
    for (auto &remove: inputCallbackRemovers)
      remove();
    for (auto it = allComponents.begin(); it != allComponents.end(); ++it)
      (*it)->recall();
    SetActive(false);
    destroyed = true;

    for (const auto child: *transform)
      child->Destroy();
  }

  void Entity2D::free() {
    allComponents.clear();
    components.clear();
    behaviours.clear();
  }
}
