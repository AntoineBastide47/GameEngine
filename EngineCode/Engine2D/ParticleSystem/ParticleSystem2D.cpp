//
// ParticleSystem2D.cpp
// Author: Antoine Bastide
// Date: 14/01/2025
//

#include <random>
#include <vector>

#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <glm/gtx/compatibility.hpp>

#include "Common/RenderingHeaders.h"
#include "Engine2D/ParticleSystem/ParticleSystem2D.h"
#include "Engine2D/Game2D.h"
#include "Engine2D/ResourceManager.h"
#include "Engine2D/Rendering/Shader.h"

namespace Engine2D {
  const float ParticleSystem2D::quadVertices[] = {
    // pos         // tex
    -0.5f, 0.5f, 0.0f, 1.0f,  // Top-left corner
    0.5f, -0.5f, 1.0f, 0.0f,  // Bottom-right corner
    -0.5f, -0.5f, 0.0f, 0.0f, // Bottom-left corner

    -0.5f, 0.5f, 0.0f, 1.0f, // Top-left corner
    0.5f, 0.5f, 1.0f, 1.0f,  // Top-right corner
    0.5f, -0.5f, 1.0f, 0.0f  // Bottom-right corner
  };

  bool ParticleSystem2D::Particle::Visible() const {
    return !(
      position.x + size.x * 0.5f < -Game2D::ViewportWidth() * 0.5f ||
      position.x - size.x * 0.5f > Game2D::ViewportWidth() * 0.5f ||
      position.y + size.y * 0.5f < -Game2D::ViewportHeight() * 0.5f ||
      position.y - size.y * 0.5f > Game2D::ViewportHeight() * 0.5f
    );
  }

  ParticleSystem2D::ParticleSystem2D()
    : loop(false), restart(false), startDelay(0), particleLifetime(1), startPosition(glm::vec2(0)),
      startVelocity(glm::vec2(0)), endVelocity(glm::vec2(0)), startAngularVelocity(0), endAngularVelocity(),
      startSize(glm::vec2(1)), endSize(glm::vec2(0)), startColor(glm::vec4(1)), endColor(glm::vec4(1)),
      simulateInWorldSpace(true), simulationSpeed(1), emissionRate(0), maxStartPositionOffset(1), duration(1),
      emissionAcc(0), durationAcc(0), simulationFinished(false), lastUsedParticle(0), instanceVBO(0), quadVAO(0), quadVBO(0),
      initialized(false), aliveCount(0) {
    this->shader = ResourceManager::GetShader("particle");
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
    this->particles.resize(maxParticles, Particle());
  }

  uint32_t ParticleSystem2D::GetMaxParticles() const {
    return this->particles.size();
  }

  void ParticleSystem2D::initialize() {
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
    // Each instance will provide: instancePosition (vec2), instanceScale (vec2), instanceColor (vec4)
    // Total stride: 8 floats.
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    // Allocate buffer with enough space for all particles.
    glBufferData(GL_ARRAY_BUFFER, particles.size() * 9 * sizeof(float), nullptr, GL_STREAM_DRAW);

    // Setup instance attributes:
    // Attribute location 1: instancePosition (vec2)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), nullptr);
    glVertexAttribDivisor(1, 1);

    // Attribute location 2: instanceScale (vec2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(2 * sizeof(float)));
    glVertexAttribDivisor(2, 1);

    // Attribute location 3: instanceColor (vec4)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(4 * sizeof(float)));
    glVertexAttribDivisor(3, 1);

    // Attribute location 4: instanceRotation (vec1)
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void *)(8 * sizeof(float)));
    glVertexAttribDivisor(4, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  void ParticleSystem2D::OnUpdate() {
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
      p.lifeTime -= dt;
      if (p.lifeTime > 0.0f) {
        const float t = p.lifeTime * dA;
        p.position -= dt * (
          useGlobalVelocities ? glm::lerp(startVelocity, endVelocity, t) : glm::lerp(p.startVelocity, p.endVelocity, t)
        );
        p.color = glm::lerp(startColor, endColor, t);
        p.size = glm::lerp(endSize, startSize, t);
        p.rotation += glm::lerp(startAngularVelocity, endAngularVelocity, t) * dt;
        ++aliveCount;
      }
    }

    simulationFinished = !loop && durationAcc > duration + startDelay && aliveCount == 0;
  }

  void ParticleSystem2D::render() {
    if ((!restart && !loop && simulationFinished) || duration == 0)
      return;

    if (!initialized)
      initialize();

    this->shader->Use();
    this->texture->bind();

    const size_t instanceDataSize = aliveCount * 9;
    const auto instanceData = new float[instanceDataSize];
    size_t i = 0;

    for (const auto &particle: particles) {
      if (particle.lifeTime <= 0.0f)
        continue;
      if (!particle.Visible())
        continue;

      const glm::vec2 scale = particle.size;
      glm::vec2 pos;
      if (simulateInWorldSpace)
        pos = particle.position;
      else
        pos = glm::vec2(Transform()->GetWorldMatrix() * glm::vec4(particle.position, 0, 1));

      instanceData[i + 0] = pos.x;
      instanceData[i + 1] = pos.y;
      instanceData[i + 2] = scale.x;
      instanceData[i + 3] = scale.y;
      instanceData[i + 4] = particle.color.r;
      instanceData[i + 5] = particle.color.g;
      instanceData[i + 6] = particle.color.b;
      instanceData[i + 7] = particle.color.a;
      instanceData[i + 8] = particle.rotation;
      i += 9;
    }

    // Update the instance VBO with new per-particle data.
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, instanceDataSize * sizeof(float), instanceData);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Bind the quad VAO and draw all instances in a single draw call.
    glBindVertexArray(quadVAO);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 6, aliveCount);
    glBindVertexArray(0);
    delete[] instanceData;
  }

  void ParticleSystem2D::respawnParticle(const uint index) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist_random(-maxStartPositionOffset, maxStartPositionOffset);

    // Get a random color.
    const float randomX = dist_random(gen);
    const float randomY = dist_random(gen);

    // Update the particle.
    Particle &particle = this->particles[index];
    particle.color = startColor;
    particle.lifeTime = particleLifetime;
    particle.velocity = startVelocity;
    particle.startVelocity = startVelocity;
    particle.endVelocity = endVelocity;
    particle.size = startSize;
    particle.position = (simulateInWorldSpace ? Transform()->GetWorldPosition() : glm::vec2(0))
                        + startPosition + glm::vec2(randomX, randomY);
    lastUsedParticle = --lastUsedParticle % particles.size();
  }
}
