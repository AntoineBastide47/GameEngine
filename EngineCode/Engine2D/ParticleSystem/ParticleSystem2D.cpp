//
// ParticleSystem2D.cpp
// Author: Antoine Bastide
// Date: 14/01/2025
//

#include <random>
#include <vector>

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
    : loop(false), restart(false), startDelay(0), startLifetime(1), startSize(Vector2f::One),
      simulateInWorldSpace(true), simulationSpeed(1), emissionRate(0), maxStartPositionOffset(1),
      duration(1), emissionAcc(0), durationAcc(0), simulationFinished(false), lastUsedParticle(0),
      instanceVBO(0), quadVAO(0), quadVBO(0), initialized(false) {
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
    emissionRate = particles.size() * 1.0f / startLifetime;
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
    glBufferData(GL_ARRAY_BUFFER, particles.size() * 8 * sizeof(float), nullptr, GL_STREAM_DRAW);

    // Setup instance attributes:
    // Attribute location 1: instancePosition (vec2)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
    glVertexAttribDivisor(1, 1);

    // Attribute location 2: instanceScale (vec2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(2 * sizeof(float)));
    glVertexAttribDivisor(2, 1);

    // Attribute location 3: instanceColor (vec4)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(4 * sizeof(float)));
    glVertexAttribDivisor(3, 1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    instanceData.reserve(particles.size() * 8);
  }

  void ParticleSystem2D::OnUpdate() {
    if ((!restart && !loop && simulationFinished) || duration == 0)
      return;

    if (restart) {
      restart = false;
      durationAcc = 0;
    }
    if (!loop || durationAcc < startDelay)
      durationAcc += Game2D::DeltaTime();
    if (particles.size() == 0 || durationAcc < startDelay || emissionRate <= 0)
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
    bool allDead = true;
    const float dA = 1.0f / startLifetime;
    for (auto &p: particles) {
      const float dt = Game2D::DeltaTime() * simulationSpeed;
      p.lifeTime -= dt;
      if (p.lifeTime > 0.0f) {
        p.position -= p.velocity * dt;
        p.color.a -= dA * dt;
        allDead = false;
      }
    }
    simulationFinished = !loop && durationAcc > duration + startDelay && allDead;
  }

  void ParticleSystem2D::render() {
    if ((!restart && !loop && simulationFinished) || duration == 0)
      return;

    if (!initialized)
      initialize();

    this->shader->Use();
    this->texture->bind();

    // Sort from youngest to oldest
    std::ranges::sort(
      particles, [](const Particle &a, const Particle &b) {
        return a.lifeTime > b.lifeTime;
      }
    );

    // Build per-instance data for all visible particles.
    // Each instance requires 8 floats: [pos.x, pos.y, scale.x, scale.y, color.r, color.g, color.b, color.a]
    for (const auto &particle: particles) {
      if (particle.lifeTime <= 0.0f || !particle.Visible())
        continue;

      glm::vec2 scale = particle.size.toGLM();
      glm::vec2 pos;
      if (simulateInWorldSpace)
        pos = particle.position.toGLM();
      else
        pos = glm::vec2(Transform()->GetWorldMatrix() * glm::vec4(particle.position.toGLM(), 0, 1));

      instanceData.push_back(pos.x);
      instanceData.push_back(pos.y);

      instanceData.push_back(scale.x);
      instanceData.push_back(scale.y);

      instanceData.push_back(particle.color.r);
      instanceData.push_back(particle.color.g);
      instanceData.push_back(particle.color.b);
      instanceData.push_back(particle.color.a);
    }

    const int instanceCount = instanceData.size() / 8;
    if (instanceCount == 0)
      return;

    // Update the instance VBO with new per-particle data.
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, instanceData.size() * sizeof(float), instanceData.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Bind the quad VAO and draw all instances in a single draw call.
    glBindVertexArray(quadVAO);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 6, instanceCount);
    glBindVertexArray(0);
    instanceData.clear();
  }

  void ParticleSystem2D::respawnParticle(const uint index) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist_random(-maxStartPositionOffset, maxStartPositionOffset);
    std::uniform_real_distribution<float> dist_rColor(0.5f, 1.5f);

    // Get a random color.
    const float randomX = dist_random(gen);
    const float randomY = dist_random(gen);
    const float rColor = dist_rColor(gen);

    // Update the particle.
    Particle &particle = this->particles[index];
    particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.lifeTime = startLifetime;
    particle.velocity = startVelocity;
    particle.size = startSize;
    particle.position = (simulateInWorldSpace ? Transform()->GetWorldPosition() : Vector2f::Zero)
                        + startPosition + Vector2f{randomX, randomY};
    lastUsedParticle = --lastUsedParticle % particles.size();
  }
}
