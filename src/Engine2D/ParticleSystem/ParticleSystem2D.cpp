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
    0.0f, 0.0f, 0.0f, 0.0f, // Bottom-left
    1.0f, 0.0f, 1.0f, 0.0f, // Bottom-right
    0.0f, 1.0f, 0.0f, 1.0f, // Top-left
    1.0f, 1.0f, 1.0f, 1.0f  // Top-right
  };

  ParticleSystem2D::ParticleSystem2D()
    : loop(false), restart(false), startDelay(0), startPosition(glm::vec2(0)), renderOrder(0), useGlobalVelocities(false),
      startVelocity(glm::vec2(0)), endVelocity(glm::vec2(0)), startAngularVelocity(0), endAngularVelocity(),
      startScale(glm::vec2(1)), endScale(glm::vec2(0)), startColor(glm::vec4(1)), endColor(glm::vec4(1)),
      simulateInWorldSpace(true), simulationSpeed(1), emissionRate(0), maxStartPositionOffset(1), duration(1),
      particleLifetime(1), inverseLifetime(1), particles(0), emissionAcc(0), durationAcc(0), simulationFinished(false),
      lastUsedParticle(0), quadVAO(0), quadVBO(0), instanceVBO(0), aliveCount(0), initialized(false) {
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
    particles = ParticleSystemData{maxParticles};

    if (emissionRate <= 0)
      emissionRate = particles.size() * inverseLifetime;
  }

  uint32_t ParticleSystem2D::GetMaxParticles() const {
    return this->particles.size();
  }

  void ParticleSystem2D::SetParticleLifetime(const float lifetime) {
    this->particleLifetime = lifetime;
    inverseLifetime = 1.0f / lifetime;
  }

  float ParticleSystem2D::GetParticleLifetime() const {
    return this->particleLifetime;
  }

  void ParticleSystem2D::initialize() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

    initialized = true;

    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STREAM_DRAW);
    // Vertex attribute 0: vertex data <vec2 vertexPos, vec2 textureCoord>
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

  void ParticleSystem2D::update() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    // Make sure we can update and render the particles
    if (const bool canUpdateAndRender = Game2D::Initialized() && Game2D::MainCamera(); !canUpdateAndRender) {
      simulationFinished = false;
      return;
    }

    if ((!restart && !loop && simulationFinished) || duration == 0 || particles.size() == 0)
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
    const auto cam = Game2D::MainCamera();
    const float dt = deltaTime * simulationSpeed;
    const glm::vec2 scaleDelta = startScale - endScale;
    const float angularVelDelta = endAngularVelocity - startAngularVelocity;
    const glm::vec2 minSize = glm::min(startScale, endScale);
    const glm::vec2 maxSize = glm::max(startScale, endScale);

    // Update alive particles
    aliveCount = 0;
    for (size_t i = 0; i < particles.size(); ++i) {
      float &lt = particles.lifeTimes[i];
      lt -= dt;
      const float t = lt * inverseLifetime;
      const glm::vec2 startVel = useGlobalVelocities ? startVelocity : particles.startVelocities[i];
      const glm::vec2 endVel = useGlobalVelocities ? endVelocity : particles.endVelocities[i];

      // Update the "transform" properties of the current particle
      glm::vec2 &position = particles.positions[i];
      position += dt * (startVel + (endVel - startVel) * t);
      particles.rotations[i] += dt * (startAngularVelocity + angularVelDelta * t);
      glm::vec2 &scale = particles.scales[i];
      scale = endScale + scaleDelta * t;

      // Check if the particle can be rendered or not
      uint8_t &renderable = particles.renderable[i];
      renderable = !(
        lt <= 0.0f || scale.x < minSize.x || scale.y < minSize.y || scale.x > maxSize.x
        || scale.y > maxSize.y || !cam->IsInViewport(position, scale)
      );
      aliveCount += renderable;
    }

    simulationFinished = !loop && durationAcc > duration + startDelay && aliveCount == 0;
  }

  void ParticleSystem2D::render(const uint32_t framebuffer) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    // Make sure we can update and render the particles
    if (const bool canUpdateAndRender = Game2D::Initialized() && Game2D::MainCamera(); !canUpdateAndRender) {
      simulationFinished = false;
      return;
    }

    if ((!restart && !loop && simulationFinished) || duration == 0 || aliveCount == 0)
      return;

    if (!initialized)
      initialize();

    shader->use();
    shader->SetMatrix4("projection", Game2D::MainCamera()->GetViewProjectionMatrix());
    sprite->texture->bind();

    // Bind the target frame buffer
    if (framebuffer > 0)
      glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, aliveCount * STRIDE_SIZE, nullptr, GL_STREAM_DRAW);

    auto gpuPtr = static_cast<float *>(glMapBufferRange(
      GL_ARRAY_BUFFER, 0, aliveCount * STRIDE_SIZE, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT
    ));

    if (!gpuPtr)
      return;

    const glm::vec4 colorDelta = endColor - startColor;
    for (size_t i = 0; i < particles.size(); ++i) {
      const glm::vec2 scale = particles.scales[i];
      if (!particles.renderable[i])
        continue;

      glm::vec2 pos;
      if (simulateInWorldSpace)
        pos = particles.positions[i];
      else
        pos = glm::vec2(Transform()->GetWorldMatrix() * glm::vec4(particles.positions[i], 0, 1));

      const float inversePPU = 1.0f / sprite->pixelsPerUnit;
      // Lerp each coordinate individually as it is faster
      const float tOpp = 1.0f - particles.lifeTimes[i] * inverseLifetime;
      glm::vec4 &color = particles.colors[i];
      color.r = startColor.r + colorDelta.r * tOpp;
      color.g = startColor.g + colorDelta.g * tOpp;
      color.b = startColor.b + colorDelta.b * tOpp;
      color.a = startColor.a + colorDelta.a * tOpp;

      // Position and scale
      *gpuPtr++ = pos.x;
      *gpuPtr++ = pos.y;
      *gpuPtr++ = scale.x * inversePPU;
      *gpuPtr++ = scale.y * inversePPU;

      // Color
      *gpuPtr++ = color.r;
      *gpuPtr++ = color.g;
      *gpuPtr++ = color.b;
      *gpuPtr++ = color.a;

      // Rect
      *gpuPtr++ = sprite->rect.x;
      *gpuPtr++ = sprite->rect.y;
      *gpuPtr++ = sprite->rect.z;
      *gpuPtr++ = sprite->rect.w;

      // Pivot, rotation and renderOrder
      *gpuPtr++ = sprite->pivot.x;
      *gpuPtr++ = sprite->pivot.y;
      *gpuPtr++ = particles.rotations[i];
      *gpuPtr++ = renderOrder;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);

    // Bind the quad VAO and draw all instances in a single draw call.
    glBindVertexArray(quadVAO);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, aliveCount);
    glBindVertexArray(0);

    // Unbind the target frame buffer
    if (framebuffer > 0)
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  static std::mt19937 gen(std::random_device{}());

  void ParticleSystem2D::respawnParticle() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerFunction);

    std::uniform_real_distribution dist_random(-maxStartPositionOffset, maxStartPositionOffset);

    particles.colors[lastUsedParticle] = startColor;
    particles.lifeTimes[lastUsedParticle] = particleLifetime;
    particles.startVelocities[lastUsedParticle] = startVelocity;
    particles.endVelocities[lastUsedParticle] = endVelocity;
    particles.scales[lastUsedParticle] = startScale;
    particles.positions[lastUsedParticle] = (simulateInWorldSpace ? Transform()->GetWorldPosition() : glm::vec2(0))
                                            + startPosition + glm::vec2(dist_random(gen), dist_random(gen));

    lastUsedParticle = lastUsedParticle == 0 ? particles.size() - 1 : lastUsedParticle - 1;
  }
}
