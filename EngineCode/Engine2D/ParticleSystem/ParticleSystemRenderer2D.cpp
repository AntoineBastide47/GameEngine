//
// ParticleSystemRenderer2D.cpp
// Author: Antoine Bastide
// Date: 16/01/2025
//

#include "Engine/RenderingHeaders.h"
#include "Engine2D/ParticleSystem/ParticleSystemRenderer2D.h"
#include "Engine2D/ParticleSystem/ParticleSystem2D.h"

namespace Engine2D {
  std::vector<std::shared_ptr<ParticleSystem2D>> ParticleSystemRenderer2D::particleSystems;

  void ParticleSystemRenderer2D::update() {
    for (const auto &particleSystem: particleSystems)
      particleSystem->update();
  }

  void ParticleSystemRenderer2D::render() {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    for (const auto &particleSystem: particleSystems)
      particleSystem->render();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  void ParticleSystemRenderer2D::addParticleSystem(const std::shared_ptr<ParticleSystem2D> &particleSystem) {
    particleSystems.push_back(particleSystem);
  }

  void ParticleSystemRenderer2D::removeParticleSystem(const std::shared_ptr<ParticleSystem2D> &particleSystem) {
    if (particleSystems.empty())
      return;
    std::erase(particleSystems, particleSystem);
  }
} // Engine2D
