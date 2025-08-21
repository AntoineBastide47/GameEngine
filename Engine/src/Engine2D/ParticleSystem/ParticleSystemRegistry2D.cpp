//
// ParticleSystemUpdater2D.cpp
// Author: Antoine Bastide
// Date: 16/01/2025
//

#include "Engine2D/ParticleSystem/ParticleSystemRegistry2D.hpp"
#include "Engine/Macros/Profiling.hpp"
#include "Engine2D/Rendering/Sprite.hpp"
#include "Engine2D/ParticleSystem/ParticleSystem2D.hpp"
#include "Engine2D/SceneManagement/Scene.hpp"

namespace Engine2D {
  ParticleSystemRegistry2D::ParticleSystemRegistry2D()
    : scene(nullptr), repartition(false), dirty(false) {}

  void ParticleSystemRegistry2D::prerender() {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerSystem);

    const size_t lastParticleCount = particleSystems.size();

    // Check if the invalid renderers are now valid
    if (!invalidParticleSystems.empty()) {
      // Sort out the invalid renderers
      auto valid_partition = std::ranges::stable_partition(
        invalidParticleSystems, [](const auto &renderer) {
          return renderer->sprite && renderer->sprite->texture;
        }
      );
      particleSystemsToAdd.insert(particleSystemsToAdd.end(), invalidParticleSystems.begin(), valid_partition.begin());
      invalidParticleSystems.erase(invalidParticleSystems.begin(), valid_partition.begin());
    }

    // Add and remove all the pending particle systems
    if (!particleSystemsToAdd.empty()) {
      // Sort out the particles system without sprites or textures
      auto invalid_partition = std::ranges::stable_partition(
        particleSystemsToAdd, [](const auto &p) {
          return p->sprite && p->sprite->texture;
        }
      );
      invalidParticleSystems.insert(invalidParticleSystems.end(), invalid_partition.begin(), invalid_partition.end());
      particleSystems.insert(particleSystems.end(), particleSystemsToAdd.begin(), invalid_partition.begin());
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

    if (particleSystems.empty())
      return;

    if (lastParticleCount != particleSystems.size() || dirty) {
      // Sort out the particles system without sprites or textures
      auto invalid_partition = std::ranges::stable_partition(
        particleSystems, [](const auto &p) {
          return p->sprite && p->sprite->texture;
        }
      );
      invalidParticleSystems.insert(invalidParticleSystems.end(), invalid_partition.begin(), invalid_partition.end());
      particleSystems.erase(invalid_partition.begin(), invalid_partition.end());

      // Repartition the particle system
      subrange = std::ranges::stable_partition(
        particleSystems, [&](const auto &p) {
          return p->sprite->transparent;
        }
      );
    }

    dirty = false;
  }

  void ParticleSystemRegistry2D::addParticleSystem(ParticleSystem2D *particleSystem) {
    if (!particleSystem)
      return;
    scene->resources.renderables.push_back(particleSystem);
    particleSystemsToAdd.emplace_back(particleSystem);
  }

  void ParticleSystemRegistry2D::removeParticleSystem(ParticleSystem2D *particleSystem) {
    if (!particleSystem)
      return;
    std::erase(scene->resources.renderables, particleSystem);
    particleSystemsToRemove.insert(particleSystem);
  }
} // Engine2D
