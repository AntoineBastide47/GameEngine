//
// Camera2D.cpp
// Author: Antoine Bastide
// Date: 20/04/2025
//

#include <glm/gtc/matrix_transform.hpp>

#include "Engine2D/Rendering/Camera2D.hpp"
#include "Engine/Macros/Profiling.hpp"
#include "Engine/Reflection/Deserializer.hpp"
#include "Engine2D/Entity2D.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/Transform2D.hpp"
#include "Engine2D/SceneManagement/SceneManager.hpp"
#include "Engine2D/Types/Vector2.hpp"

namespace Engine2D::Rendering {
  Camera2D::Camera2D()
    : Camera2D(-1, 1, -1, 1) {}

  Camera2D::Camera2D(const float left, const float right, const float bottom, const float top)
    : positionOffset(0), rotationOffset(0), damping(1), zoomLevel(1.0f),
      projection(glm::ortho(left, right, bottom, top, near, far)), view(1.0f), initialized(false), shaking(false),
      shakeDuration(0), shakeElapsed(0), ubo(0), followTargetIndex(-1), velocity(),
      rotationMatrix(), worldRotationLastFrame(std::numeric_limits<float>::quiet_NaN()), invZoom(1), left(left),
      right(right), bottom(bottom), top(top) {
    viewProjection = projection * view;
  }

  Camera2D::~Camera2D() {
    if (ubo > 0) {
      glDeleteBuffers(1, &ubo);
      ubo = 0;
    }
  }

  void Camera2D::Shake(const float duration) {
    shaking = true;
    shakeElapsed = 0;
    shakeDuration = duration;
  }

  bool Camera2D::IsInViewport(const glm::vec2 &position, const glm::vec2 &scale) const {
    const auto rotated = rotationMatrix * (position - Transform()->WorldPosition());
    const float halfWidth = scale.x * 0.5f;
    const float halfHeight = scale.y * 0.5f;

    constexpr float cullMargin = 2.0f;
    const float zoomedLeft = left * invZoom - cullMargin;
    const float zoomedRight = right * invZoom + cullMargin;
    const float zoomedBottom = bottom * invZoom - cullMargin;
    const float zoomedTop = top * invZoom + cullMargin;

    return !(
      rotated.x + halfWidth < zoomedLeft ||
      rotated.x - halfWidth > zoomedRight ||
      rotated.y + halfHeight < zoomedBottom ||
      rotated.y - halfHeight > zoomedTop
    );
  }

  void Camera2D::Resize(const float width, const float height) {
    constexpr float baseZoom = 60.0f;
    constexpr float baseAspect = 16.0f / 9.0f; // TODO: read this from settings

    if (const float currentAspect = width / height; currentAspect > baseAspect) {
      // Window is wider than base - fit to height
      constexpr float viewHeight = baseZoom;
      const float viewWidth = viewHeight * currentAspect;
      SetProjection(-viewWidth * 0.5f, viewWidth * 0.5f, -viewHeight * 0.5f, viewHeight * 0.5f);
    } else {
      // Window is taller than base - fit to width
      constexpr float viewWidth = baseZoom * baseAspect;
      const float viewHeight = viewWidth / currentAspect;
      SetProjection(-viewWidth * 0.5f, viewWidth * 0.5f, -viewHeight * 0.5f, viewHeight * 0.5f);
    }
  }

  void Camera2D::SetProjection(const float left, const float right, const float bottom, const float top) {
    projection = glm::ortho(left, right, bottom, top, near, far);
    viewProjection = projection * view;

    this->left = left;
    this->right = right;
    this->bottom = bottom;
    this->top = top;
  }

  const glm::mat4 &Camera2D::ProjectionMatrix() const {
    return projection;
  }

  const glm::mat4 &Camera2D::ViewMatrix() const {
    return view;
  }

  const glm::mat4 &Camera2D::ViewProjectionMatrix() const {
    return viewProjection;
  }

  inline int index_of_ptr(const std::vector<std::unique_ptr<Entity2D>> &vec, const Entity2D *ptr) {
    if (!ptr)
      return -1;

    for (size_t i = 0; i < vec.size(); ++i)
      if (vec[i].get() == ptr)
        return i;
    return -1;
  }

  void Camera2D::OnSerialize(const Engine::Reflection::Format format, Engine::JSON &json) const {
    if (format == Engine::Reflection::Format::JSON)
      json["followTarget"] = index_of_ptr(SceneManager::ActiveScene()->entities, followTarget.get());
  }

  void Camera2D::OnDeserialize(const Engine::Reflection::Format format, const Engine::JSON &json) {
    if (format == Engine::Reflection::Format::JSON)
      followTargetIndex = static_cast<int>(json.At("followTarget").GetNumber());

    SetProjection(left, right, bottom, top);
  }

  void Camera2D::initialize() {
    // Bind the engine data ubo
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Set the binding port
    glBindBufferBase(GL_UNIFORM_BUFFER, ENGINE_DATA_BINDING_PORT, ubo);

    initialized = true;
  }

  void Camera2D::updateCamera() {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerSubSystem);

    if (!initialized)
      initialize();

    /// Update the camera's transform
    if (followTarget) {
      const glm::vec2 target = followTarget->Transform()->WorldPosition() + positionOffset;
      Transform()->SetPosition(
        glm::smooth_damp(
          Transform()->WorldPosition(), target, velocity, std::clamp(damping, 1e-6f, 1.0f), Game2D::DeltaTime()
        )
      );
      const auto increment = glm::degrees(followTarget->Transform()->WorldRotation());
      Transform()->SetRotation(rotationOffset + increment);
      if (followTarget == Entity())
        Transform()->UpdateRotation(-increment);
    }

    // Compute the view so that we can apply the camera shake to it
    // First negate the world rotation because of matrix inversion
    Transform()->worldRotation = -Transform()->worldRotation;
    glm::mat4 baseView = glm::scale(glm::inverse(Transform()->WorldMatrix()), glm::vec3(zoomLevel, zoomLevel, 1.0f));
    Transform()->worldRotation = -Transform()->worldRotation;

    // Rotate into camera space
    if (const auto cameraRotation = Transform()->WorldRotation(); cameraRotation != worldRotationLastFrame) {
      const float c = std::cos(-cameraRotation);
      const float s = std::sin(-cameraRotation);
      rotationMatrix = glm::mat2(c, s, -s, c);
      worldRotationLastFrame = cameraRotation;
    }

    if (const auto inv = 1.0f / zoomLevel; inv != invZoom) {
      invZoom = 1.0f / zoomLevel;
      SceneManager::ActiveScene()->makeAllEntitiesDirty();
    }

    // Run camera shake
    if (shaking) {
      shakeElapsed += Game2D::DeltaTime();
      if (const float dt = shakeElapsed / shakeDuration; dt < 1.0f)
        baseView = glm::translate(glm::mat4(1.0f), glm::vec3(getCameraShake(dt), 0.0f)) * baseView;
      else
        shaking = false;
    }

    // Update the camera's matrices
    view = baseView;
    viewProjection = projection * view;

    // Send the engine data to shaders
    void *ptr = glMapBufferRange(
      GL_UNIFORM_BUFFER, 0, 3 * sizeof(glm::mat4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT
    );
    if (ptr) {
      memcpy(ptr, &view, sizeof(glm::mat4));
      memcpy(static_cast<char *>(ptr) + sizeof(glm::mat4), &projection, sizeof(glm::mat4));
      memcpy(static_cast<char *>(ptr) + 2 * sizeof(glm::mat4), &viewProjection, sizeof(glm::mat4));
      glUnmapBuffer(GL_UNIFORM_BUFFER);
    }
  }

  glm::vec2 Camera2D::getCameraShake(const float frac) const {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerFunction);

    if (frac <= 0.0f || frac >= 1.0f)
      return glm::vec2(0.0f);

    constexpr float mid = 0.25f;

    float x = 0.0f, y = 0.0f;

    for (const auto &[amplitude, frequency, phase]: shakeCoefficientsX)
      x += amplitude * std::sin(frequency * glm::pi<float>() * frac + phase);

    for (const auto &[amplitude, frequency, phase]: shakeCoefficientsY)
      y += amplitude * std::sin(frequency * glm::pi<float>() * frac + phase);

    const float lim = frac <= mid ? frac / mid : 1.0f - (frac - mid) / (1.0f - mid);
    return glm::vec2(x, y) * lim;
  }

  #if ENGINE_EDITOR
  bool Camera2D::OnRenderInEditor(const std::string &name, bool, const bool readOnly) {
    bool changed = Engine::Reflection::_e_renderInEditorImpl(followTarget, "Follow Target", readOnly);

    if (followTarget) {
      changed |= Engine::Reflection::_e_renderInEditorImpl(positionOffset, "Position Offset", readOnly);
      changed |= Engine::Reflection::_e_renderInEditorImpl(rotationOffset, "Rotation Offset", readOnly);
    }

    changed |= Engine::Reflection::_e_renderInEditorImpl(damping, "Damping", readOnly);
    changed |= Engine::Reflection::_e_renderInEditorImpl(zoomLevel, "Zoom Level", readOnly);
    changed |= Engine::Reflection::_e_renderInEditorImpl(shakeDuration, "Shake Duration", readOnly);
    changed |= Engine::Reflection::_e_renderInEditorImpl(shakeCoefficientsX, "Shake Coefficient X", readOnly);
    changed |= Engine::Reflection::_e_renderInEditorImpl(shakeCoefficientsY, "Shake Coefficient Y", readOnly);
    return changed;
  }
  #endif
}
