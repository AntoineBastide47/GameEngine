//
// ParticleSystemRenderer2D.cpp
// Author: Antoine Bastide
// Date: 16/01/2025
//

#include "Engine/RenderingHeaders.hpp"
#include "Engine2D/ParticleSystem/ParticleSystemRenderer2D.hpp"

#include "Engine/Macros/Profiling.hpp"
#include "Engine2D/ParticleSystem/ParticleSystem2D.hpp"

namespace Engine2D {
  std::vector<std::shared_ptr<ParticleSystem2D>> ParticleSystemRenderer2D::particleSystems;
  std::vector<std::shared_ptr<ParticleSystem2D>> ParticleSystemRenderer2D::particleSystemsToAdd;
  std::vector<std::shared_ptr<ParticleSystem2D>> ParticleSystemRenderer2D::particleSystemsToRemove;
  #if MULTI_THREAD
  std::mutex ParticleSystemRenderer2D::particleSystemMutex;
  #endif

  void ParticleSystemRenderer2D::update() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    #if MULTI_THREAD
    std::lock_guard lock(particleSystemMutex);
    #endif

    for (const auto &particleSystem: particleSystems)
      particleSystem->update();
  }

  void ParticleSystemRenderer2D::render() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    #if MULTI_THREAD
    std::lock_guard lock(particleSystemMutex);
    #endif

    // Add and remove all the pending particle systems
    for (const auto &particleSystem: particleSystemsToAdd)
      particleSystems.emplace_back(particleSystem);
    particleSystemsToAdd.clear();
    for (const auto &particleSystem: particleSystemsToRemove)
      std::erase(particleSystems, particleSystem);
    particleSystemsToRemove.clear();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for (const auto &particleSystem: particleSystems)
      particleSystem->render();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  void ParticleSystemRenderer2D::addParticleSystem(const std::shared_ptr<ParticleSystem2D> &particleSystem) {
    particleSystemsToAdd.emplace_back(particleSystem);
  }

  void ParticleSystemRenderer2D::removeParticleSystem(const std::shared_ptr<ParticleSystem2D> &particleSystem) {
    particleSystemsToRemove.emplace_back(particleSystem);
  }
} // Engine2D
