//
// Entity.cpp
// Author: Antoine Bastide
// Date: 03/11/2024
//

#include <ranges>

#include "Engine2D/Entity2D.hpp"
#include "Engine/Reflection/Deserializer.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/Rendering/SpriteRenderer.hpp"
#include "Engine2D/SceneManagement/SceneManager.hpp"

namespace Engine2D {
  Entity2D::Entity2D()
    : Entity2D("Entity", false, {0, 0}, 0, {0, 0}, nullptr) {}

  Entity2D::Entity2D(
    std::string name, const bool isStatic, const glm::vec2 position, const float rotation, const glm::vec2 scale,
    Entity2D *parent
  )
    : name(std::move(name)), active(true), parentsActive(true), isStatic(isStatic), destroyed(false),
      timeToLive(0.1f), id(0),
      transform(std::unique_ptr<Transform2D>(new Transform2D(position, rotation, scale, this, parent))),
      scene(nullptr) {}

  Entity2D::~Entity2D() {
    destroy();
    free();
  }

  bool Entity2D::operator==(const Entity2D &entity) const {
    return id == entity.id;
  }

  bool Entity2D::operator!=(const Entity2D &entity) const {
    return !(*this == entity);
  }

  Transform2D *Entity2D::Transform() const {
    return transform.get();
  }

  uint64_t Entity2D::Id() const {
    return id;
  }

  Entity2D::operator uint64_t() const {
    return id;
  }

  Entity2D *Entity2D::Instantiate(
    const std::string &name, const bool isStatic, const glm::vec2 position, const float rotation, const glm::vec2 scale,
    Entity2D *parent
  ) {
    const auto entity = new Entity2D(name, isStatic, position, rotation, scale, parent);
    entity->id = nextId++;
    SceneManager::ActiveScene()->addEntity(std::unique_ptr<Entity2D>(entity));
    return entity;
  }

  void Entity2D::OnSerialize(Engine::Reflection::Format format, Engine::JSON &json) const {
    json["id"] = std::to_string(id);
  }

  void Entity2D::SetActive(const bool active) {
    if (this->active == active)
      return;

    this->active = active;
    for (const auto child: *transform) {
      if (child && child->transform) {
        child->transform->onParentHierarchyChange();

        if (const auto renderer = child->GetComponent<Rendering::SpriteRenderer>())
          renderer->dirty = true;
      }
    }

    if (const auto renderer = GetComponent<Rendering::SpriteRenderer>())
      renderer->dirty = true;
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

  void Entity2D::initialize() {
    transform->entity = this;
    transform->onTransformChange();
    if (!transform->parent)
      transform->SetParent(nullptr);
    else {
      transform->parent->transform->addChild(this);
      transform->onParentHierarchyChange();
    }
  }

  void Entity2D::destroy() {
    for (auto &remove: inputCallbackRemovers)
      remove();
    for (auto it = allComponents.begin(); it != allComponents.end(); ++it)
      (*it)->recall();
    active = false;
    destroyed = true;

    if (transform) {
      for (const auto child: *transform) {
        child->Transform()->parent = nullptr;
        child->destroy();
      }
      if (transform->parent && transform->parent->transform)
        transform->parent->transform->removeChild(this);
      transform->parent = nullptr;
      transform->children.clear();
    }
  }

  void Entity2D::free() {
    allComponents.clear();
    components.clear();
    behaviours.clear();
    transform.reset();
  }

  void Entity2D::forceSetParent(Entity2D *parent) {
    const bool isStatic = this->isStatic;
    this->isStatic = false;
    transform->SetParent(parent);
    this->isStatic = isStatic;
  }

  void Entity2D::onDeserialize(
    const Engine::Reflection::Format format, const Engine::JSON &json, Engine2D::Scene *scene
  ) {
    this->scene = scene;
    transform->entity = this;
    transform->SetPositionRotationAndScale(transform->worldPosition, transform->worldRotation, transform->worldScale);
    const int index = static_cast<int>(json.At("transform").At("data").At("parent").GetNumber());
    forceSetParent(index == -1 ? nullptr : (scene->entities.begin() + index)->get());

    char *end;
    id = std::strtoull(json.At("id").GetString().c_str(), &end, 10);
    nextId = id + 1;

    std::erase(allComponents, nullptr);
    for (auto &component: allComponents) {
      component->entity = this;
      if (const auto behaviour = dynamic_cast<Behaviour *>(component.get()); behaviour) {
        behaviour->OnBindInput();
        behaviours.emplace_back(behaviour);
      } else {
        component->forward();
        components.emplace_back(component.get());
      }
    }

    int j = 0;
    for (const auto &component: allComponents)
      component->OnDeserialize(format, json.At("allComponents").At(j++).At("data"));
  }
}
