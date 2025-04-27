//
// ParticleSystemRenderer2D.cpp
// Author: Antoine Bastide
// Date: 16/01/2025
//

#include "Engine/RenderingHeaders.hpp"
#include "Engine2D/ParticleSystem/ParticleSystemRenderer2D.hpp"
#include "Engine2D/ParticleSystem/ParticleSystem2D.hpp"
#include "Engine/Macros/Profiling.hpp"

namespace Engine2D {
  std::vector<std::shared_ptr<ParticleSystem2D>> ParticleSystemRenderer2D::particleSystems;
  std::vector<std::shared_ptr<ParticleSystem2D>> ParticleSystemRenderer2D::particleSystemsToAdd;
  std::unordered_set<std::shared_ptr<ParticleSystem2D>> ParticleSystemRenderer2D::particleSystemsToRemove;
  #if MULTI_THREAD
  std::mutex ParticleSystemRenderer2D::particleSystemMutex;
  #endif

  void ParticleSystemRenderer2D::update() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    #if MULTI_THREAD
    std::lock_guard lock(particleSystemMutex);
    #endif

    // Add and remove all the pending particle systems
    if (!particleSystemsToAdd.empty()) {
      particleSystems.insert(particleSystems.end(), particleSystemsToAdd.begin(), particleSystemsToAdd.end());
      particleSystemsToAdd.clear();
    }
    // remove in one pass
    if (!particleSystemsToRemove.empty()) {
      std::erase_if(
        particleSystems, [&](auto &ps) {
          return particleSystemsToRemove.count(ps);
        }
      );
      particleSystemsToRemove.clear();
    }

    for (const auto &particleSystem: particleSystems)
      particleSystem->update();
  }

  void ParticleSystemRenderer2D::render() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    #if MULTI_THREAD
    std::lock_guard lock(particleSystemMutex);
    #endif

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    for (const auto &particleSystem: particleSystems)
      particleSystem->render();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  void ParticleSystemRenderer2D::addParticleSystem(const std::shared_ptr<ParticleSystem2D> &particleSystem) {
    particleSystemsToAdd.emplace_back(particleSystem);
  }

  void ParticleSystemRenderer2D::removeParticleSystem(const std::shared_ptr<ParticleSystem2D> &particleSystem) {
    particleSystemsToRemove.insert(particleSystem);
  }
} // Engine2D
