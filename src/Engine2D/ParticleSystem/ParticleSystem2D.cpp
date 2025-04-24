//
// ParticleSystem2D.cpp
// Author: Antoine Bastide
// Date: 14/01/2025
//

#define STRIDE (16)
#define STRIDE_SIZE (STRIDE * sizeof(float))

#include <vector>

#include "Engine/RenderingHeaders.hpp"
#include "Engine2D/ParticleSystem/ParticleSystem2D.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine/ResourceManager.hpp"
#include "Engine/Rendering/Shader.hpp"
#include "Engine/Rendering/Texture.hpp"
#include "Engine2D/Rendering/Camera2D.hpp"
#include "Engine2D/Rendering/Sprite.hpp"

namespace Engine2D {
  const float ParticleSystem2D::quadVertices[] = {
    // pos      // tex
    0.0f, 1.0f, 0.0f, 1.0f, // Top-left
    1.0f, 0.0f, 1.0f, 0.0f, // Bottom-right
    0.0f, 0.0f, 0.0f, 0.0f, // Bottom-left

    0.0f, 1.0f, 0.0f, 1.0f, // Top-left
    1.0f, 1.0f, 1.0f, 1.0f, // Top-right
    1.0f, 0.0f, 1.0f, 0.0f  // Bottom-right
  };

  bool ParticleSystem2D::Particle::Visible() {
    return visible = Game2D::Initialized() && Game2D::MainCamera() && Game2D::MainCamera()->IsInViewport(
                       position, scale
                     );
  }

  ParticleSystem2D::ParticleSystem2D()
    : loop(false), restart(false), startDelay(0), particleLifetime(1), startPosition(glm::vec2(0)), renderOrder(0),
      useGlobalVelocities(false), startVelocity(glm::vec2(0)), endVelocity(glm::vec2(0)), startAngularVelocity(0),
      endAngularVelocity(), startScale(glm::vec2(1)), endScale(glm::vec2(0)), startColor(glm::vec4(1)),
      endColor(glm::vec4(1)),
      simulateInWorldSpace(true), simulationSpeed(1), emissionRate(0), maxStartPositionOffset(1), duration(1),
      emissionAcc(0), durationAcc(0), simulationFinished(false), lastUsedParticle(0), quadVAO(0), quadVBO(0), instanceVBO(0),
      aliveCount(0), initialized(false) {
    this->shader = Engine::ResourceManager::GetShader("particle");
  }

  void ParticleSystem2D::SetDuration(const float duration) {
    this->duration = duration;
    this->durationAcc = 0;
  }

  float ParticleSystem2D::GetDuration() const {
    return this->duration;
  }

  // Prevent reallocation if the particle system has already been used.
  void ParticleSystem2D::SetMaxParticles(const size_t maxParticles) {
    if (instanceVBO)
      return;
    this->particles.resize(maxParticles);
  }

  uint32_t ParticleSystem2D::GetMaxParticles() const {
    return this->particles.size();
  }

  void ParticleSystem2D::initialize() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

    initialized = true;

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STREAM_DRAW);
    // Vertex attribute 0: vertex data (vec4: position.xy, texCoords.xy)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    // Create the instance VBO for per-particle data.
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * STRIDE_SIZE, nullptr, GL_STREAM_DRAW);

    // Setup instance attributes:
    constexpr GLsizei vec4Size = 4 * sizeof(float);
    // Attribute location 1: position and scale
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, nullptr);
    glVertexAttribDivisor(1, 1);

    // Attribute location 2: color
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, reinterpret_cast<void *>(vec4Size));
    glVertexAttribDivisor(2, 1);

    // Attribute location 3: rect
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, reinterpret_cast<void *>(2 * vec4Size));
    glVertexAttribDivisor(3, 1);

    // Attribute location 4: pivot, rotation and renderOrder
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, reinterpret_cast<void *>(3 * vec4Size));
    glVertexAttribDivisor(4, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  bool ParticleSystem2D::validParticle(const Particle &p) const {
    const glm::vec2 minSize = glm::min(startScale, endScale);
    const glm::vec2 maxSize = glm::max(startScale, endScale);

    return p.lifeTime > 0.0f &&
           p.scale.x >= minSize.x && p.scale.y >= minSize.y &&
           p.scale.x <= maxSize.x && p.scale.y <= maxSize.y;
  }

  void ParticleSystem2D::update() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    if ((!restart && !loop && simulationFinished) || duration == 0 || particles.size() == 0)
      return;

    if (emissionRate <= 0)
      emissionRate = particles.size() / particleLifetime;

    if (restart) {
      restart = false;
      durationAcc = 0;
    }
    if (!loop || durationAcc < startDelay)
      durationAcc += Game2D::DeltaTime();
    if (durationAcc < startDelay)
      return;

    if (loop || durationAcc <= duration + startDelay) {
      emissionAcc += emissionRate * Game2D::DeltaTime();
      const int newParticles = static_cast<int>(emissionAcc);
      emissionAcc -= newParticles;

      // Add new particles.
      for (size_t i = 0; i < newParticles; ++i)
        this->respawnParticle(lastUsedParticle);
    }

    // Update all particles.
    aliveCount = 0;
    const float dA = 1.0f / particleLifetime;
    const float dt = Game2D::DeltaTime() * simulationSpeed;

    for (auto &p: particles) {
      if (p.lifeTime <= 0.0f)
        continue;

      p.lifeTime -= dt;
      if (validParticle(p)) {
        const float t = p.lifeTime * dA;
        p.color = glm::mix(startColor, endColor, 1.0f - t);
        p.scale = glm::mix(endScale, startScale, t);
        p.rotation += glm::mix(startAngularVelocity, endAngularVelocity, t) * dt;

        p.position += dt * glm::mix(
          useGlobalVelocities ? startVelocity : p.startVelocity,
          useGlobalVelocities ? endVelocity : p.endVelocity,
          t
        );

        if (p.Visible())
          ++aliveCount;
      }
    }

    simulationFinished = !loop && durationAcc > duration + startDelay && aliveCount == 0;
  }

  void ParticleSystem2D::render() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    if ((!restart && !loop && simulationFinished) || duration == 0)
      return;

    if (!initialized)
      initialize();

    shader->use();
    shader->SetMatrix4("projection", Game2D::MainCamera()->GetViewProjectionMatrix());
    sprite->texture->bind();

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * STRIDE_SIZE, nullptr, GL_STREAM_DRAW);

    auto gpuPtr = static_cast<float *>(glMapBufferRange(GL_ARRAY_BUFFER, 0, aliveCount * STRIDE_SIZE, GL_MAP_WRITE_BIT));

    if (!gpuPtr)
      return;

    for (const auto &particle: particles) {
      if (!validParticle(particle) || !particle.visible)
        continue;

      const glm::vec2 scale = particle.scale;
      glm::vec2 pos;
      if (simulateInWorldSpace)
        pos = particle.position;
      else
        pos = glm::vec2(Transform()->GetWorldMatrix() * glm::vec4(particle.position, 0, 1));

      // Position and scale
      *gpuPtr++ = pos.x;
      *gpuPtr++ = pos.y;
      *gpuPtr++ = scale.x / sprite->pixelsPerUnit;
      *gpuPtr++ = scale.y / sprite->pixelsPerUnit;

      // Color
      *gpuPtr++ = particle.color.r;
      *gpuPtr++ = particle.color.g;
      *gpuPtr++ = particle.color.b;
      *gpuPtr++ = particle.color.a;

      // Rect
      *gpuPtr++ = sprite->rect.x;
      *gpuPtr++ = sprite->rect.y;
      *gpuPtr++ = sprite->rect.z;
      *gpuPtr++ = sprite->rect.w;

      // Pivot, rotation and renderOrder
      *gpuPtr++ = sprite->pivot.x;
      *gpuPtr++ = sprite->pivot.y;
      *gpuPtr++ = particle.rotation;
      *gpuPtr++ = renderOrder;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    // Bind the quad VAO and draw all instances in a single draw call.
    glBindVertexArray(quadVAO);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 6, aliveCount);
    glBindVertexArray(0);
  }

  void ParticleSystem2D::respawnParticle(const uint index) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution dist_random(-maxStartPositionOffset, maxStartPositionOffset);

    Particle &particle = this->particles[index];
    particle.color = startColor;
    particle.lifeTime = particleLifetime;
    particle.startVelocity = startVelocity;
    particle.endVelocity = endVelocity;
    particle.scale = startScale;
    particle.position = (simulateInWorldSpace ? Transform()->GetWorldPosition() : glm::vec2(0))
                        + startPosition + glm::vec2(dist_random(gen), dist_random(gen));
    lastUsedParticle = --lastUsedParticle % particles.size();
  }
}
