//
// ParticleSystem2D.cpp
// Author: Antoine Bastide
// Date: 14/01/2025
//

#include <random>
#include <vector>

#include "Engine2D/ParticleSystem/ParticleSystem2D.hpp"
#include "Engine/RenderingHeaders.hpp"
#include "Engine/Macros/Profiling.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/Transform2D.hpp"
#include "Engine2D/ParticleSystem/ParticleSystemRegistry2D.hpp"
#include "Engine2D/Rendering/Camera2D.hpp"
#include "Engine2D/Rendering/Renderer2D.hpp"
#include "Engine2D/Rendering/Sprite.hpp"
#include "Engine2D/SceneManagement/SceneManager.hpp"

#define STRIDE (16)
#define STRIDE_SIZE (STRIDE * sizeof(float))

namespace Engine2D {
  ParticleSystem2D::ParticleSystem2D()
    : Renderable2D(ParticleSystem), loop(false), restart(false), useGlobalVelocities(false), simulateInWorldSpace(true),
      startDelay(0), startPosition(glm::vec2(0)), flip({false, false}), startVelocity(glm::vec2(0)),
      endVelocity(glm::vec2(0)), startAngularVelocity(0), endAngularVelocity(), startScale(glm::vec2(1)),
      endScale(glm::vec2(0)), startColor(glm::vec4(1)), endColor(glm::vec4(1)), simulationSpeed(1), emissionRate(0),
      maxStartPositionOffset(1), blendMode(Alpha), duration(1), particleLifetime(1), inverseLifetime(1), particles(0),
      head(0), capacity(0), maxParticles(0), simulationFinished(false), emissionAcc(0), durationAcc(0), quadVAO(0),
      quadVBO(0), instanceVBO(0) {}

  void ParticleSystem2D::SetDuration(const float duration) {
    this->duration = duration;
    durationAcc = 0;
  }

  float ParticleSystem2D::Duration() const {
    return duration;
  }

  // Prevent reallocation if the particle system has already been used.
  void ParticleSystem2D::SetMaxParticles(const int maxParticles, const bool uniformEmissionRate) {
    if (maxParticles < 0) {
      this->maxParticles = 0;
      return;
    }

    particles.reserve(maxParticles);
    particles.resize(maxParticles);
    head = (head + maxParticles - 1) % (maxParticles == 0 ? 1 : maxParticles);
    if (capacity > maxParticles)
      capacity = maxParticles;
    this->maxParticles = maxParticles;

    if (uniformEmissionRate || emissionRate <= 0)
      emissionRate = maxParticles * inverseLifetime;
  }

  uint32_t ParticleSystem2D::MaxParticles() const {
    return maxParticles;
  }

  void ParticleSystem2D::SetParticleLifetime(const float lifetime) {
    particleLifetime = lifetime;
    inverseLifetime = 1.0f / lifetime;
  }

  float ParticleSystem2D::ParticleLifetime() const {
    return particleLifetime;
  }

  void ParticleSystem2D::OnDeserialize(Engine::Reflection::Format format, const Engine::JSON &json) {
    Renderable2D::OnDeserialize(format, json);
    SetMaxParticles(maxParticles);
    SetParticleLifetime(particleLifetime);
  }

  void ParticleSystem2D::forward() {
    Entity()->Scene()->particleSystemRegistry.addParticleSystem(this);
  }

  void ParticleSystem2D::recall() {
    Entity()->Scene()->particleSystemRegistry.removeParticleSystem(this);
  }

  void ParticleSystem2D::updateAndRender(const uint textureIndex, float *data) {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerSystem);

    // Make sure we can update and render the particles
    if (const bool canUpdateAndRender = Game2D::Initialized() && SceneManager::ActiveScene()->MainCamera(); !
      canUpdateAndRender) {
      simulationFinished = false;
      return;
    }

    if ((!restart && !loop && simulationFinished) || duration == 0 || maxParticles == 0)
      return;

    if (restart) {
      restart = false;
      durationAcc = 0;
    }

    const float deltaTime = Game2D::DeltaTime();
    durationAcc += deltaTime;
    if (!loop && durationAcc < startDelay)
      return;

    if (loop || durationAcc <= duration + startDelay) {
      emissionAcc += emissionRate * deltaTime;
      const size_t newParticles = emissionAcc;
      emissionAcc -= newParticles;

      // Add new particles.
      for (size_t i = 0; i < newParticles; ++i)
        respawnParticle();
    }

    // Precompute reused data
    const auto &cam = *SceneManager::ActiveScene()->MainCamera();
    const float dt = deltaTime * simulationSpeed;
    const glm::vec2 scaleDelta = startScale - endScale;
    const float angularVelDelta = endAngularVelocity - startAngularVelocity;
    const glm::vec4 colorDelta = endColor - startColor;

    int j = 0;
    for (int i = 0; i < capacity; ++i) {
      int index = head + capacity - 1 - i;
      if (index >= maxParticles)
        index -= maxParticles;
      if (index < 0)
        index += maxParticles;
      auto &[position, scale, startVelocity, endVelocity, rotation, lifeTime] = particles[index];
      lifeTime -= dt;

      const float t = lifeTime * inverseLifetime;
      const glm::vec2 startVel = useGlobalVelocities ? startVelocity : startVelocity;
      const glm::vec2 endVel = useGlobalVelocities ? endVelocity : endVelocity;

      // Update the "transform" properties of the current particle
      position += dt * (startVel + (endVel - startVel) * t);
      rotation += dt * (startAngularVelocity + angularVelDelta * t);
      scale = endScale + scaleDelta * t;

      if (lifeTime <= 0.0f && !cam.IsInViewport(position, scale))
        continue;

      glm::vec2 pos;
      if (simulateInWorldSpace)
        pos = position;
      else
        pos = glm::vec2(Transform()->WorldMatrix() * glm::vec4(position, 0, 1));

      const int idx = j * STRIDE;
      const float inversePPU = 1.0f / sprite->pixelsPerUnit;
      const float tOpp = 1.0f - lifeTime * inverseLifetime;

      // Position and scale
      data[idx + 0] = pos.x;
      data[idx + 1] = pos.y;
      data[idx + 2] = scale.x * inversePPU;
      data[idx + 3] = scale.y * inversePPU;

      // Color
      data[idx + 4] = startColor.r + colorDelta.r * tOpp;
      data[idx + 5] = startColor.g + colorDelta.g * tOpp;
      data[idx + 6] = startColor.b + colorDelta.b * tOpp;
      data[idx + 7] = startColor.a + colorDelta.a * tOpp;

      // Rect
      data[idx + 8] = std::clamp(sprite->rect.x, 0.0f, 1.0f);
      data[idx + 9] = std::clamp(sprite->rect.y, 0.0f, 1.0f);
      data[idx + 10] = std::clamp(sprite->rect.z, 0.0f, 1.0f);
      data[idx + 11] = std::clamp(sprite->rect.w, 0.0f, 1.0f);

      // Pivot, rotation and renderOrder
      data[idx + 12] = Rendering::Renderer2D::PackTwoFloats(
        std::clamp(sprite->pivot.x, -1.0f, 1.0f),
        std::clamp(sprite->pivot.y, -1.0f, 1.0f)
      );
      data[idx + 13] = rotation;
      data[idx + 14] = renderOrder << 16 | textureIndex;
      data[idx + 15] = Rendering::Renderer2D::PackTwoFloats(flip.x, flip.y);

      j++;
    }

    // Remove dead particles
    while (capacity > 0) {
      int index = head + capacity - 1;
      if (index >= maxParticles)
        index -= maxParticles;
      if (index < 0)
        index += maxParticles;

      if (const Particle &back = particles[index];
        back.lifeTime > 0.0f)
        break;
      --capacity;
    }

    simulationFinished = !loop && durationAcc > duration + startDelay && capacity == 0;
  }

  static std::mt19937 gen(std::random_device{}());

  void ParticleSystem2D::respawnParticle() {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerFunction);

    std::uniform_real_distribution dist_random(-maxStartPositionOffset, maxStartPositionOffset);

    head = head == 0 ? maxParticles - 1 : head - 1;

    Particle &particle = particles[head];
    particle.lifeTime = particleLifetime;
    particle.startVelocity = startVelocity;
    particle.endVelocity = endVelocity;
    particle.scale = startScale;
    particle.position = (simulateInWorldSpace ? Transform()->WorldPosition() : glm::vec2(0))
                        + startPosition + glm::vec2(dist_random(gen), dist_random(gen));

    if (capacity < maxParticles)
      ++capacity;
  }

  void ParticleSystem2D::OnEditorValueChanged() {
    SetDuration(duration);
    SetMaxParticles(maxParticles);
    SetParticleLifetime(particleLifetime);
  }
}
