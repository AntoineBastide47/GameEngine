//
// Transform2D.cpp
// Author: Antoine Bastide
// Date: 03/11/2024
//

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

#include "Engine2D/Transform2D.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine/Log.hpp"
#include "Engine/Macros/Profiling.hpp"
#include "Engine2D/Physics/Collider2D.hpp"
#include "Engine2D/Rendering/Camera2D.hpp"

namespace Engine2D {
  Transform2D::Transform2D()
    : Transform2D({0, 0}, 0, {0, 0}, nullptr) {}

  Transform2D::Transform2D(
    const glm::vec2 position, const float rotation, const glm::vec2 scale, Entity2D *entity, Entity2D *parent
  )
    : position(position), rotation(rotation), scale(scale), parent(parent), dirty(true), visible(true), worldPosition(),
      worldRotation(0), worldScale(), projectionMatrix(glm::mat4(1.0f)) {
    setEntity(entity);
  }

  bool Transform2D::operator==(const Transform2D &other) const {
    return parent == other.parent && position == other.position && rotation == other.rotation && scale == other.scale;
  }

  bool Transform2D::operator!=(const Transform2D &transform) const {
    return !(*this == transform);
  }

  std::vector<Entity2D *>::iterator Transform2D::begin() {
    return childList.begin();
  }

  std::vector<Entity2D *>::iterator Transform2D::end() {
    return childList.end();
  }

  std::vector<Entity2D *>::const_iterator Transform2D::begin() const {
    return childList.cbegin();
  }

  std::vector<Entity2D *>::const_iterator Transform2D::end() const {
    return childList.cend();
  }

  glm::vec2 Transform2D::GetPosition() const {
    return position;
  }

  glm::vec2 Transform2D::GetScale() const {
    return scale;
  }

  float Transform2D::GetRotation() const {
    return rotation;
  }

  void Transform2D::SetPosition(const glm::vec2 newPosition) {
    SetPositionRotationAndScale(newPosition, rotation, scale);
  }

  void Transform2D::SetRotation(const float newRotation) {
    SetPositionRotationAndScale(position, newRotation, scale);
  }

  void Transform2D::SetScale(const glm::vec2 newScale) {
    SetPositionRotationAndScale(position, rotation, newScale);
  }

  void Transform2D::SetPositionAndRotation(const glm::vec2 newPosition, const float newRotation) {
    SetPositionRotationAndScale(newPosition, newRotation, scale);
  }

  void Transform2D::SetPositionAndScale(const glm::vec2 newPosition, const glm::vec2 newScale) {
    SetPositionRotationAndScale(newPosition, rotation, newScale);
  }

  void Transform2D::SetRotationAndScale(const float newRotation, const glm::vec2 newScale) {
    SetPositionRotationAndScale(position, newRotation, newScale);
  }

  void Transform2D::SetPositionRotationAndScale(
    const glm::vec2 newPosition, const float newRotation, const glm::vec2 newScale
  ) {
    if (!Entity()->isStatic) {
      position = newPosition;
      rotation = fmod(newRotation, 360.0f);
      scale = newScale;
      onTransformChange();
    }
  }

  void Transform2D::UpdatePosition(const glm::vec2 positionIncrement) {
    UpdatePositionRotationAndScale(positionIncrement, 0, glm::vec2(0));
  }

  void Transform2D::UpdateScale(const glm::vec2 scaleIncrement) {
    UpdatePositionRotationAndScale(glm::vec2(0), 0, scaleIncrement);
  }

  void Transform2D::UpdateRotation(const float rotationIncrement) {
    UpdatePositionRotationAndScale(glm::vec2(0), rotationIncrement, glm::vec2(0));
  }

  void Transform2D::UpdatePositionAndRotation(const glm::vec2 positionIncrement, const float rotationIncrement) {
    UpdatePositionRotationAndScale(positionIncrement, rotationIncrement, glm::vec2(0));
  }

  void Transform2D::UpdatePositionAndScale(const glm::vec2 positionIncrement, const glm::vec2 scaleIncrement) {
    UpdatePositionRotationAndScale(positionIncrement, 0, scaleIncrement);
  }

  void Transform2D::UpdateRotationAndScale(const float rotationIncrement, const glm::vec2 scaleIncrement) {
    UpdatePositionRotationAndScale(glm::vec2(0), rotationIncrement, scaleIncrement);
  }

  void Transform2D::UpdatePositionRotationAndScale(
    const glm::vec2 positionIncrement, const float rotationIncrement, const glm::vec2 scaleIncrement
  ) {
    if (!Entity()->isStatic) {
      position += positionIncrement;
      rotation = fmod(rotation + rotationIncrement, 360.0f);
      scale += scaleIncrement;
      onTransformChange();
    }
  }

  glm::vec2 Transform2D::Forward() const {
    return glm::normalize(glm::rotate(glm::vec2(1, 0), glm::radians(rotation)));
  }

  glm::vec2 Transform2D::Up() const {
    return glm::normalize(glm::rotate(glm::vec2(0, 1), glm::radians(rotation)));
  }

  void Transform2D::SetParent(Entity2D *parent) {
    if (Entity()->isStatic)
      return;

    if (this->parent == parent)
      return;

    if (this->parent)
      this->parent->transform->removeChild(Entity());

    dirty = true;
    this->parent = parent;
    onParentHierarchyChange();

    // If no parent is specified, set the parent to null
    if (!parent) {
      position = worldPosition;
      rotation = worldRotation;
      scale = worldScale;
      return;
    }

    // Create the parent hierarchy
    this->parent->transform->addChild(Entity());

    // Convert the transform properties from world space to local space
    auto localTransform = glm::inverse(this->parent->transform->GetWorldMatrix()) * GetWorldMatrix();
    position = glm::vec2(localTransform[3][0], -localTransform[3][1]);
    rotation = scale.x == 0
                 ? 0
                 : glm::degrees(std::atan2(localTransform[1][0] / scale.x, localTransform[0][0] / scale.x));
    scale.x = glm::length(glm::vec2(localTransform[0][0], localTransform[1][0]));
    scale.y = glm::length(glm::vec2(localTransform[0][1], localTransform[1][1]));
  }

  Entity2D *Transform2D::GetParent() const {
    return parent;
  }

  bool Transform2D::IsChildOf(const Entity2D *entity) const {
    const Transform2D *current = parent ? parent->transform.get() : nullptr;

    while (current) {
      if (current->Entity() && current->Entity() == entity)
        return true;
      current = current->parent ? current->parent->transform.get() : nullptr;
    }
    return false;
  }

  bool Transform2D::IsParentOf(const Entity2D *entity) const {
    for (const auto &child: childList)
      if (child == entity)
        return true;
    return false;
  }

  glm::vec2 Transform2D::GetWorldPosition() const {
    return worldPosition;
  }

  float Transform2D::GetWorldRotation() const {
    return worldRotation;
  }

  glm::vec2 Transform2D::GetWorldScale() const {
    return worldScale;
  }

  glm::vec2 Transform2D::GetWorldHalfScale() const {
    return worldScale * 0.5f;
  }

  void Transform2D::RemoveAllChildren() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSubSystem);

    for (const auto child: childList)
      if (child)
        child->transform->SetParent(nullptr);
    childList.clear();
  }

  Entity2D *Transform2D::Find(const std::string &name) const {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

    for (const auto child: childList)
      if (child && child->name == name)
        return child;
    return nullptr;
  }

  Entity2D *Transform2D::GetChild(const int index) const {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

    if (index < 0 || index >= childList.size())
      return Engine::Log::Error("Transform2D::GetChild: index out of bounds");
    return childList.at(index);
  }

  void Transform2D::MakeFirstChild(Entity2D *child) {
    removeChild(child);
    childList.insert(childList.begin(), child);
  }

  void Transform2D::MakeLastChild(Entity2D *child) {
    removeChild(child);
    childList.push_back(child);
  }

  bool Transform2D::GetIsVisible() const {
    return visible;
  }

  const glm::mat4 &Transform2D::GetWorldMatrix() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSubSystem);
    if (dirty) {
      projectionMatrix =
          glm::translate(glm::mat4(1.0f), glm::vec3(worldPosition, 0.0f)) *
          glm::rotate(glm::mat4(1.0f), -glm::radians(worldRotation), glm::vec3(0.0f, 0.0f, 1.0f)) *
          glm::scale(glm::mat4(1.0f), glm::vec3(worldScale, 1.0f));
      dirty = false;
    }
    return projectionMatrix;
  }

  glm::vec2 Transform2D::WorldToLocal(const glm::vec2 &point) {
    return glm::vec2(glm::vec2(glm::inverse(GetWorldMatrix()) * glm::vec4(point, 0.0f, 1.0f)));
  }

  void Transform2D::onTransformChange() {
    const auto oldWorldPosition = worldPosition;
    const auto oldWorldRotation = worldRotation;
    const auto oldWorldScale = worldScale;

    worldPosition = position;
    worldRotation = rotation;
    worldScale = scale;

    const Transform2D *current = parent ? parent->transform.get() : nullptr;
    while (current) {
      worldPosition = glm::rotate(worldPosition * current->scale, glm::radians(-current->rotation)) + current->position;
      worldRotation += current->rotation;
      worldScale = worldScale * current->scale;
      current = current->parent ? current->parent->transform.get() : nullptr;
    }

    if (oldWorldPosition == worldPosition && oldWorldRotation == worldRotation && oldWorldScale == worldScale)
      return;

    // Determine if the sprite is currently visible to the camera
    dirty = true;
    visible = Game2D::Initialized() && Game2D::MainCamera() &&
              Game2D::MainCamera()->IsInViewport(worldPosition, worldScale);

    // Update the children of this transform as they depend on the transform of their parent
    for (const auto child: childList)
      if (child)
        child->transform->onTransformChange();
  }

  void Transform2D::onParentHierarchyChange() const {
    // Check if the entity has a parent
    if (!parent) {
      Entity()->parentsActive = true;
      return;
    }

    // If it is active, make sure all it's parents are also active
    const Transform2D *current = parent->transform.get();
    while (current && current->Entity()) {
      if (!current->Entity()->active) {
        Entity()->parentsActive = false;
        return;
      }
      current = current->parent ? current->parent->transform.get() : nullptr;
    }
    Entity()->parentsActive = true;
  }

  void Transform2D::addChild(Entity2D *child) {
    if (child)
      childList.push_back(child);
  }

  void Transform2D::removeChild(Entity2D *child) {
    if (child)
      std::erase(childList, child);
  }
}
