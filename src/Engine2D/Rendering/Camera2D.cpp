//
// Camera2D.cpp
// Author: Antoine Bastide
// Date: 20/04/2025
//

#include <glm/gtc/matrix_transform.hpp>

#include "Engine2D/Rendering/Camera2D.hpp"
#include "Engine/Macros/Profiling.hpp"
#include "Engine2D/Entity2D.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/Transform2D.hpp"
#include "Engine2D/Types/Vector2.hpp"

namespace Engine2D::Rendering {
  uint Camera2D::ENGINE_DATA_BINDING_PORT = 0;

  Camera2D::Camera2D(
    const float left, const float right, const float bottom, const float top, const float near, const float far
  )
    : positionOffset(0), rotationOffset(0), damping(0), projection(glm::ortho(left, right, bottom, top, near, far)),
      view(1.0f), shaking(false), initialized(false), shakeElapsed(0), shakeDuration(0), ubo(0), m00(0), m01(0), m03(0),
      m10(0), m11(0), m13(0) {
    viewProjection = projection * view;
  }

  void Camera2D::Shake(const float duration) {
    shaking = true;
    shakeElapsed = 0;
    shakeDuration = duration;
  }

  bool Camera2D::IsInViewport(const glm::vec2 &position, const glm::vec2 &scale) const {
    // Project the sprite center into clip space
    const float cx = m00 * position.x + m01 * position.y + m03;
    const float cy = m10 * position.x + m11 * position.y + m13;

    // Compute sprite’s world‐space bounding‐circle radius (half‐diagonal)
    const float hx = scale.x * 0.5f;
    const float hy = scale.y * 0.5f;
    const float worldRadius = hx * hx + hy * hy;

    // Compute max clip‐space scale factor (to map world -> clip uniformly)
    const float sx = m00 * m00 + m10 * m10;
    const float sy = m01 * m01 + m11 * m11;
    const float clipScale = sx > sy ? sx : sy;
    const float r = worldRadius * clipScale;

    // Check the bounds of the circle using squared distances
    if (cx > 1.0f && (cx - 1.0f) * (cx - 1.0f) > r)
      return false;
    if (cx < -1.0f && (cx + 1.0f) * (cx + 1.0f) > r)
      return false;
    if (cy > 1.0f && (cy - 1.0f) * (cy - 1.0f) > r)
      return false;
    if (cy < -1.0f && (cy + 1.0f) * (cy + 1.0f) > r)
      return false;
    return true;
  }

  const glm::mat4 &Camera2D::GetProjectionMatrix() const {
    return this->projection;
  }

  const glm::mat4 &Camera2D::GetViewMatrix() const {
    return this->view;
  }

  const glm::mat4 &Camera2D::GetViewProjectionMatrix() const {
    return this->viewProjection;
  }

  void Camera2D::initialize() {
    // Bind the engine data ubo
    glGenBuffers(1, &ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo);
    glBufferData(GL_UNIFORM_BUFFER, 3 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // Set the binding port
    glBindBufferBase(GL_UNIFORM_BUFFER, ENGINE_DATA_BINDING_PORT, ubo);

    initialized = true;
  }

  void Camera2D::updateCamera() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSubSystem);

    if (!initialized)
      initialize();

    /// Update the camera's transform
    if (followTarget) {
      auto velocity = glm::vec2(0);
      const glm::vec2 target = followTarget->transform->GetWorldPosition() + positionOffset;
      Transform()->SetPosition(
        glm::smoothDamp(Transform()->GetWorldPosition(), target, velocity, damping, Game2D::DeltaTime())
      );
      Transform()->SetRotation(rotationOffset + followTarget->transform->GetWorldRotation());
    }

    // Compute the view so that we can apply the camera shake to it
    glm::mat4 baseView = glm::inverse(Transform()->GetWorldMatrix());

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

    m00 = viewProjection[0][0];
    m01 = viewProjection[1][0];
    m03 = viewProjection[3][0];
    m10 = viewProjection[0][1];
    m11 = viewProjection[1][1];
    m13 = viewProjection[3][1];

    // Send the engine data to shaders
    void *ptr = glMapBufferRange(
      GL_UNIFORM_BUFFER, 0, 3 * sizeof(glm::mat4), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT
    );
    memcpy(ptr, &view, sizeof(glm::mat4));
    memcpy(static_cast<char *>(ptr) + sizeof(glm::mat4), &projection, sizeof(glm::mat4));
    memcpy(static_cast<char *>(ptr) + 2 * sizeof(glm::mat4), &viewProjection, sizeof(glm::mat4));
    glUnmapBuffer(GL_UNIFORM_BUFFER);
  }

  glm::vec2 Camera2D::getCameraShake(const float frac) const {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

    if (frac <= 0.0f || frac >= 1.0f)
      return glm::vec2(0.0f);

    constexpr float mid = 0.25f;

    float x = 0.0f, y = 0.0f;

    for (const auto &[amplitude, frequency, phase]: shakeCoefficientsX)
      x += amplitude * std::sin(frequency * glm::pi<float>() * frac + phase);

    for (const auto &[amplitude, frequency, phase]: shakeCoefficientsY)
      y += amplitude * std::sin(frequency * glm::pi<float>() * frac + phase);

    const float lim = (frac <= mid) ? frac / mid : 1.0f - (frac - mid) / (1.0f - mid);
    return glm::vec2(x, y) * lim;
  }
}
