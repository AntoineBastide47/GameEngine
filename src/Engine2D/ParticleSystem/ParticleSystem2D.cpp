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
#include "Engine2D/SceneManager.hpp"
#include "Engine2D/Transform2D.hpp"
#include "Engine2D/ParticleSystem/ParticleSystemRegistry2D.hpp"
#include "Engine2D/Rendering/Camera2D.hpp"
#include "Engine2D/Rendering/Renderer2D.hpp"
#include "Engine2D/Rendering/Sprite.hpp"

#define STRIDE (16)
#define STRIDE_SIZE (STRIDE * sizeof(float))

namespace Engine2D {
  ParticleSystem2D::ParticleSystem2D()
    : Renderable2D(ParticleSystem), loop(false), restart(false), useGlobalVelocities(false), simulateInWorldSpace(true),
      startDelay(0), startPosition(glm::vec2(0)), startVelocity(glm::vec2(0)), endVelocity(glm::vec2(0)),
      startAngularVelocity(0), endAngularVelocity(), startScale(glm::vec2(1)), endScale(glm::vec2(0)),
      startColor(glm::vec4(1)), endColor(glm::vec4(1)), simulationSpeed(1), emissionRate(0), maxStartPositionOffset(1),
      blendMode(Alpha), duration(1), particleLifetime(1), inverseLifetime(1), particles(0), head(0), maxParticles(0),
      aliveCount(0), emissionAcc(0), durationAcc(0), quadVAO(0), quadVBO(0), instanceVBO(0), capacity(0),
      simulationFinished(false) {}

  void ParticleSystem2D::SetDuration(const float duration) {
    this->duration = duration;
    this->durationAcc = 0;
  }

  float ParticleSystem2D::GetDuration() const {
    return this->duration;
  }

  // Prevent reallocation if the particle system has already been used.
  void ParticleSystem2D::SetMaxParticles(const size_t maxParticles, const bool uniformEmissionRate) {
    particles.reserve(maxParticles);
    particles.resize(maxParticles);
    head = (head + maxParticles - 1) % maxParticles;
    if (capacity > maxParticles)
      capacity = maxParticles;
    this->maxParticles = maxParticles;

    if (uniformEmissionRate || emissionRate <= 0)
      emissionRate = maxParticles * inverseLifetime;
  }

  uint32_t ParticleSystem2D::GetMaxParticles() const {
    return this->maxParticles;
  }

  void ParticleSystem2D::SetParticleLifetime(const float lifetime) {
    this->particleLifetime = lifetime;
    inverseLifetime = 1.0f / lifetime;
  }

  float ParticleSystem2D::GetParticleLifetime() const {
    return this->particleLifetime;
  }

  void ParticleSystem2D::forward() {
    SceneManager::ActiveScene()->particleSystemRegistry.addParticleSystem(this);
  }

  void ParticleSystem2D::recall() {
    SceneManager::ActiveScene()->particleSystemRegistry.removeParticleSystem(this);
  }

  void ParticleSystem2D::updateAndRender(const uint textureIndex, float *data) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

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
        this->respawnParticle();
    }

    // Precompute reused data
    const auto &cam = *Scene::MainCamera();
    const float dt = deltaTime * simulationSpeed;
    const glm::vec2 scaleDelta = startScale - endScale;
    const float angularVelDelta = endAngularVelocity - startAngularVelocity;
    const glm::vec4 colorDelta = endColor - startColor;
    aliveCount = 0;

    int j = 0;
    for (size_t i = 0; i < capacity; ++i) {
      int index = head + capacity - 1 - i;
      if (index >= maxParticles)
        index -= maxParticles;
      if (index < 0)
        index += maxParticles;
      Particle &particle = particles[index];
      particle.lifeTime -= dt;

      const float t = particle.lifeTime * inverseLifetime;
      const glm::vec2 startVel = useGlobalVelocities ? startVelocity : particle.startVelocity;
      const glm::vec2 endVel = useGlobalVelocities ? endVelocity : particle.endVelocity;

      // Update the "transform" properties of the current particle
      particle.position += dt * (startVel + (endVel - startVel) * t);
      particle.rotation += dt * (startAngularVelocity + angularVelDelta * t);
      particle.scale = endScale + scaleDelta * t;

      if (particle.lifeTime <= 0.0f && !cam.IsInViewport(particle.position, particle.scale))
        continue;

      aliveCount++;
      glm::vec2 pos;
      if (simulateInWorldSpace)
        pos = particle.position;
      else
        pos = glm::vec2(Transform()->GetWorldMatrix() * glm::vec4(particle.position, 0, 1));

      const int idx = j * STRIDE;
      const float inversePPU = 1.0f / sprite->pixelsPerUnit;
      const float tOpp = 1.0f - particle.lifeTime * inverseLifetime;

      // Position and scale
      data[idx + 0] = pos.x;
      data[idx + 1] = pos.y;
      data[idx + 2] = particle.scale.x * inversePPU;
      data[idx + 3] = particle.scale.y * inversePPU;

      // Color
      data[idx + 4] = startColor.r + colorDelta.r * tOpp;
      data[idx + 5] = startColor.g + colorDelta.g * tOpp;
      data[idx + 6] = startColor.b + colorDelta.b * tOpp;
      data[idx + 7] = startColor.a + colorDelta.a * tOpp;

      // Rect
      data[idx + 8] = sprite->rect.x;
      data[idx + 9] = sprite->rect.y;
      data[idx + 10] = sprite->rect.z;
      data[idx + 11] = sprite->rect.w;

      // Pivot, rotation and renderOrder
      data[idx + 12] = Rendering::Renderer2D::PackTwoFloats(sprite->pivot.x, sprite->pivot.y);
      data[idx + 13] = particle.rotation;
      data[idx + 14] = renderOrder;
      data[idx + 15] = textureIndex;

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
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

    std::uniform_real_distribution dist_random(-maxStartPositionOffset, maxStartPositionOffset);

    head = head == 0 ? maxParticles - 1 : head - 1;

    Particle &particle = particles[head];
    particle.lifeTime = particleLifetime;
    particle.startVelocity = startVelocity;
    particle.endVelocity = endVelocity;
    particle.scale = startScale;
    particle.position = (simulateInWorldSpace ? Transform()->GetWorldPosition() : glm::vec2(0))
                        + startPosition + glm::vec2(dist_random(gen), dist_random(gen));

    if (capacity < maxParticles)
      ++capacity;
  }
}
