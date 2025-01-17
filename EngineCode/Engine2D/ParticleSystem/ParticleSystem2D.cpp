//
// ParticleSystem2D.cpp
// Author: Antoine Bastide
// Date: 14/01/2025
//

#include <random>

#include "Common/RenderingHeaders.h"
#include "Engine2D/ParticleSystem/ParticleSystem2D.h"
#include "Engine2D/Game2D.h"
#include "Engine2D/Rendering/Shader.h"

namespace Engine2D {
  bool Particle::Visible() const {
    return !(
      position.x + size.x * 0.5f < -Game2D::ViewportWidth() * 0.5f ||
      position.x - size.x * 0.5f > Game2D::ViewportWidth() * 0.5f ||
      position.y + size.y * 0.5f < -Game2D::ViewportHeight() * 0.5f ||
      position.y - size.y * 0.5f > Game2D::ViewportHeight() * 0.5f
    );
  }

  ParticleSystem2D::ParticleSystem2D()
    : duration(
        Engine::Property{
          1.0f, [this] {
            this->durationAcc = 0;
          }
        }
      ), loop(false), startDelay(0), startLifetime(1), startSize(Vector2f::One), simulateInWorldSpace(true),
      simulationSpeed(1), maxParticles(
        Engine::Property<unsigned int>{
          1, [this] {
            this->particles.resize(this->maxParticles, Particle());
          }
        }
      ), emissionRate(0), maxStartPositionOffset(1), emissionAcc(0), durationAcc(0), skipUpdate(false),
      simulationFinished(false), forceSkipUpdateAndRendering(false), lastUsedParticle(0) {
    this->shader = ResourceManager::GetShader("particle");
  }

  void ParticleSystem2D::OnUpdate() {
    if (forceSkipUpdateAndRendering)
      return;

    durationAcc += Game2D::DeltaTime();
    skipUpdate = maxParticles.Get() == 0 || durationAcc < startDelay;
    if (skipUpdate)
      return;

    if (emissionRate <= 0.0f)
      emissionRate = maxParticles.Get() * 1.0f / startLifetime;

    simulationFinished = !loop && duration + startDelay <= durationAcc;
    if (!simulationFinished) {
      emissionAcc += emissionRate * Game2D::DeltaTime();
      const int newParticles = static_cast<int>(emissionAcc);
      emissionAcc -= newParticles;

      // add new particles
      for (unsigned int i = 0; i < newParticles; ++i)
        this->respawnParticle(lastUsedParticle);
    }

    // update all particles
    bool allDead = true;
    for (auto &p: particles) {
      const float dt = Game2D::DeltaTime() * simulationSpeed;
      p.duration -= dt;
      if (p.duration > 0.0f) {
        p.position -= p.velocity * dt;
        p.color.a -= dt;
        allDead = false;
      }
    }
    simulationFinished = simulationFinished && allDead;
  }

  void ParticleSystem2D::render() {
    if (forceSkipUpdateAndRendering || !texture)
      return;

    this->shader->Use();
    this->texture->bind();

    for (unsigned int i = 0; i < particles.size(); ++i) {
      const auto &particle = particles[i];
      if (particle.duration <= 0.0f) {
        if (i < lastUsedParticle)
          lastUsedParticle = i;
        continue;
      }
      if (!particle.Visible())
        continue;

      this->shader->SetVector2f(
        "position",
        simulateInWorldSpace
          ? particle.position.toGLM()
          : Transform()->WorldMatrix() * glm::vec4(particle.position.toGLM(), 0, 1)
      );
      this->shader->SetVector2f("scale", particle.size.toGLM());
      this->shader->SetVector4f("color", glm::vec4(1, 1, 1, 1));
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
    }

    if (simulationFinished)
      forceSkipUpdateAndRendering = true;
  }

  void ParticleSystem2D::respawnParticle(const unsigned int index) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution dist_random(-maxStartPositionOffset, maxStartPositionOffset);
    std::uniform_real_distribution dist_rColor(0.5f, 1.5f);

    // Get a random color
    const float randomX = dist_random(gen);
    const float randomY = dist_random(gen);
    const float rColor = dist_rColor(gen);

    // Update the particle
    Particle &particle = this->particles[index];
    particle.color = glm::vec4(rColor, rColor, rColor, 1.0f);
    particle.duration = startLifetime;
    particle.velocity = startVelocity;
    particle.size = startSize;
    particle.position = (simulateInWorldSpace ? Transform()->WorldPosition() : Vector2f::Zero) + startPosition + Vector2f{randomX, randomY};
    lastUsedParticle++;
  }
}
