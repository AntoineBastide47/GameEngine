//
// Transform2D.cpp
// Author: Antoine Bastide
// Date: 03/11/2024
//

#include <iostream>

#include "Engine2D/Transform2D.h"
#include "Engine2D/Entity2D.h"
#include "Engine2D/Game2D.h"
#include "Common/Log.h"
#include "Engine2D/Physics/Collisions.h"

namespace Engine2D {
  Transform2D::Transform2D(const Vector2f position, const float rotation, const Vector2f scale)
    : position(position), rotation(std::fmod(rotation, 360.0f)), scale(scale), parent(nullptr), isDirty(true),
      visible(true), projectionMatrix(glm::mat4(1.0f)) {
    onTransformChange();
  }

  Transform2D::~Transform2D() {
    RemoveAllChildren();
    parent = nullptr;
  }

  bool Transform2D::operator==(const Transform2D &other) const {
    return parent == other.parent && position == other.position && rotation == other.rotation && scale == other.scale;
  }

  bool Transform2D::operator!=(const Transform2D &transform) const {
    return !(*this == transform);
  }

  std::vector<std::shared_ptr<Entity2D>>::iterator Transform2D::begin() {
    return childList.begin();
  }

  std::vector<std::shared_ptr<Entity2D>>::iterator Transform2D::end() {
    return childList.end();
  }

  std::vector<std::shared_ptr<Entity2D>>::const_iterator Transform2D::begin() const {
    return childList.cbegin();
  }

  std::vector<std::shared_ptr<Entity2D>>::const_iterator Transform2D::end() const {
    return childList.cend();
  }

  Vector2f Transform2D::GetPosition() const {
    return position;
  }

  Vector2f Transform2D::GetScale() const {
    return scale;
  }

  float Transform2D::GetRotation() const {
    return rotation;
  }

  void Transform2D::SetPosition(const Vector2f newPosition) {
    this->position = newPosition;
    onTransformChange();
  }

  void Transform2D::SetRotation(const float newRotation) {
    this->rotation = fmod(newRotation, 360.0f);
    onTransformChange();
  }

  void Transform2D::SetScale(const Vector2f newScale) {
    this->scale = newScale;
    onTransformChange();
  }

  void Transform2D::SetPositionAndRotation(const Vector2f newPosition, const float newRotation) {
    this->position = newPosition;
    this->rotation = fmod(newRotation, 360.0f);
    onTransformChange();
  }

  void Transform2D::SetPositionAndScale(const Vector2f newPosition, const Vector2f newScale) {
    this->position = newPosition;
    this->scale = newScale;
    onTransformChange();
  }

  void Transform2D::SetRotationAndScale(const float newRotation, const Vector2f newScale) {
    this->rotation = fmod(newRotation, 360.0f);
    this->scale = newScale;
    onTransformChange();
  }

  void Transform2D::SetPositionRotationAndScale(
    const Vector2f newPosition, const float newRotation, const Vector2f newScale
  ) {
    this->position = newPosition;
    this->rotation = fmod(newRotation, 360.0f);
    this->scale = newScale;
    onTransformChange();
  }

  void Transform2D::UpdatePosition(const Vector2f positionIncrement) {
    this->position += positionIncrement;
    onTransformChange();
  }

  void Transform2D::UpdateScale(const Vector2f scaleIncrement) {
    this->scale += scaleIncrement;
    onTransformChange();
  }

  void Transform2D::UpdateRotation(const float rotationIncrement) {
    this->rotation = fmod(this->rotation + rotationIncrement, 360.0f);
    onTransformChange();
  }

  void Transform2D::UpdatePositionAndRotation(const Vector2f positionIncrement, const float rotationIncrement) {
    this->position += positionIncrement;
    this->rotation = fmod(this->rotation + rotationIncrement, 360.0f);
    onTransformChange();
  }

  void Transform2D::UpdatePositionAndScale(const Vector2f positionIncrement, const Vector2f scaleIncrement) {
    this->position += positionIncrement;
    this->scale += scaleIncrement;
    onTransformChange();
  }

  void Transform2D::UpdateRotationAndScale(const float rotationIncrement, const Vector2f scaleIncrement) {
    this->rotation = fmod(this->rotation + rotationIncrement, 360.0f);
    this->scale += scaleIncrement;
    onTransformChange();
  }

  void Transform2D::UpdatePositionRotationAndScale(
    const Vector2f positionIncrement, const float rotationIncrement, const Vector2f scaleIncrement
  ) {
    this->position += positionIncrement;
    this->rotation = fmod(this->rotation + rotationIncrement, 360.0f);
    this->scale += scaleIncrement;
    isDirty = true;
  }

  Vector2f Transform2D::Forward() const {
    return Vector2f::Right.Rotated(rotation).Normalized();
  }

  Vector2f Transform2D::Up() const {
    return Vector2f::Up.Rotated(rotation).Normalized();
  }

  void Transform2D::SetParent(const std::shared_ptr<Entity2D> &parent) {
    if (this->parent == parent)
      return;

    if (this->parent)
      this->parent->transform.removeChild(Entity());

    // If no parent is specified, set the parent to null
    if (!parent) {
    position = GetWorldPosition();
    rotation = GetWorldRotation();
    scale = GetWorldScale();
      this->parent = nullptr;
      onParentHierarchyChange();
      return;
    }

    // Create the parent hierarchy
    this->parent = parent;
    this->parent->transform.addChild(Entity());
    onParentHierarchyChange();

    // Convert the transform properties from world space to local space
    auto localTransform = glm::inverse(this->parent->transform.GetWorldMatrix()) * GetWorldMatrix();
    position = Vector2f{localTransform[3][0], -localTransform[3][1]};
    rotation = scale.x == 0 ? 0 : glm::degrees(std::atan2(localTransform[1][0] / scale.x, localTransform[0][0] / scale.x));
    scale.x = Vector2f{localTransform[0][0], localTransform[1][0]}.Magnitude();
    scale.y = Vector2f{localTransform[0][1], localTransform[1][1]}.Magnitude();
  }

  std::shared_ptr<Entity2D> Transform2D::GetParent() const {
    return parent;
  }

  bool Transform2D::IsChildOf(const std::shared_ptr<Entity2D> &entity) const {
    Transform2D *current = parent ? &parent->transform : nullptr;

    while (current) {
      if (current->Entity() && current->Entity() == entity)
        return true;
      current = current->parent ? &current->parent->transform : nullptr;
    }
    return false;
  }

  bool Transform2D::IsParentOf(const std::shared_ptr<Entity2D> &entity) const {
    for (const auto &child: childList)
      if (child == entity)
        return true;
    return false;
  }

  Vector2f Transform2D::GetWorldPosition() const {
    return worldPosition;
  }

  float Transform2D::GetWorldRotation() const {
    return worldRotation;
  }

  Vector2f Transform2D::GetWorldScale() const {
    return worldScale;
  }

  Vector2f Transform2D::GetWorldHalfScale() const {
    return worldScaleHalf;
  }

  void Transform2D::RemoveAllChildren() {
    for (const auto child: childList)
      if (child)
        child->transform.SetParent(nullptr);
    childList.clear();
  }

  std::shared_ptr<Entity2D> Transform2D::Find(const std::string &name) const {
    for (auto child: childList)
      if (child && child->name == name)
        return child;
    return nullptr;
  }

  std::shared_ptr<Entity2D> Transform2D::GetChild(const int index) const {
    if (index < 0 || index >= childList.size())
      return Engine::Log::Error("Transform2D::GetChild: index out of bounds");
    return childList.at(index);
  }

  void Transform2D::MakeFirstChild(const std::shared_ptr<Entity2D> &child) {
    this->removeChild(child);
    childList.insert(childList.begin(), child);
  }

  void Transform2D::MakeLastChild(const std::shared_ptr<Entity2D> &child) {
    this->removeChild(child);
    childList.push_back(child);
  }

  bool Transform2D::GetIsVisible() const {
    return this->visible;
  }

  glm::mat4 Transform2D::GetWorldMatrix() {
    if (isDirty) {
      projectionMatrix =
        glm::translate(glm::mat4(1.0f), glm::vec3(worldPosition.toGLM(), 0.0f)) *
        glm::rotate(glm::mat4(1.0f), -glm::radians(worldRotation), glm::vec3(0.0f, 0.0f, 1.0f)) *
        glm::scale(glm::mat4(1.0f), glm::vec3(worldScale.Scaled({1, -1}).toGLM(), 1.0f));
      isDirty = false;
    }
    return this->projectionMatrix;
  }

  Vector2f Transform2D::WorldToLocal(const Vector2f &point) {
    return Vector2f{glm::vec2(glm::inverse(GetWorldMatrix()) * glm::vec4(point.toGLM(), 0.0f, 1.0f))};
  }

  void Transform2D::onTransformChange() {
    const auto oldWorldPosition = this->worldPosition;
    const auto oldWorldRotation = this->worldRotation;
    const auto oldWorldScale = this->worldScale;

    worldPosition = position;
    worldRotation = rotation;
    worldScale = scale;

    const Transform2D *current = parent ? &parent->transform : nullptr;
    while (current) {
      worldPosition = worldPosition.Scaled(current->scale).Rotated(-current->rotation) + current->position;
      worldRotation += current->rotation;
      worldScale = worldScale.Scaled(current->scale);
      current = current->parent ? &current->parent->transform : nullptr;
    }
    worldScaleHalf = worldScale * 0.5f;

    if (oldWorldPosition == worldPosition && oldWorldRotation == worldRotation && oldWorldScale == worldScale)
      return;

    isDirty = true;
    visible = Game2D::instance && !(
                worldPosition.x + worldScaleHalf.x < -Game2D::ViewportWidth() * 0.5f ||
                worldPosition.x - worldScaleHalf.x > Game2D::ViewportWidth() * 0.5f ||
                worldPosition.y + worldScaleHalf.y < -Game2D::ViewportHeight() * 0.5f ||
                worldPosition.y - worldScaleHalf.y > Game2D::ViewportHeight() * 0.5f
              );

    // Update the children of this transform as they depend on the transform of their parent
    for (const auto child: childList)
      if (child)
        child->transform.onTransformChange();
  }

  void Transform2D::onParentHierarchyChange() {
    // Check if the entity has a parent
    if (!parent) {
      Entity()->parentsActive = true;
      return;
    }
    // If it is active, make sure all it's parents are also active
    Transform2D *current = &parent->transform;
    while (current && current->Entity()) {
      if (!current->Entity()->active) {
        Entity()->parentsActive = false;
        return;
      }
      current = current->parent ? &current->parent->transform : nullptr;
    }
    Entity()->parentsActive = true;
  }

  void Transform2D::addChild(const std::shared_ptr<Entity2D> &child) {
    if (child)
      childList.push_back(child);
  }

  void Transform2D::removeChild(const std::shared_ptr<Entity2D> &child) {
    if (!child)
      return;
    if (const auto it = std::ranges::find(childList, child); it != childList.end())
      childList.erase(it);
  }
}
