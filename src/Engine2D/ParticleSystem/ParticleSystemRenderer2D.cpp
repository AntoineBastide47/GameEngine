//
// ParticleSystemRenderer2D.cpp
// Author: Antoine Bastide
// Date: 16/01/2025
//

#include "Engine/RenderingHeaders.hpp"
#include "Engine2D/ParticleSystem/ParticleSystemRenderer2D.hpp"
#include "Engine2D/ParticleSystem/ParticleSystem2D.hpp"

namespace Engine2D {
  std::vector<std::shared_ptr<ParticleSystem2D>> ParticleSystemRenderer2D::particleSystems;
  std::mutex ParticleSystemRenderer2D::particleSystemMutex;

  void ParticleSystemRenderer2D::update() {
    std::lock_guard lock(particleSystemMutex);

    for (const auto &particleSystem: particleSystems)
      particleSystem->update();
  }

  void ParticleSystemRenderer2D::render() {
    std::lock_guard lock(particleSystemMutex);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for (const auto &particleSystem: particleSystems)
      particleSystem->render();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  void ParticleSystemRenderer2D::addParticleSystem(const std::shared_ptr<ParticleSystem2D> &particleSystem) {
    std::lock_guard lock(particleSystemMutex);
    particleSystems.push_back(particleSystem);
  }

  void ParticleSystemRenderer2D::removeParticleSystem(const std::shared_ptr<ParticleSystem2D> &particleSystem) {
    std::lock_guard lock(particleSystemMutex);
    if (particleSystems.empty())
      return;
    std::erase(particleSystems, particleSystem);
  }
} // Engine2D
