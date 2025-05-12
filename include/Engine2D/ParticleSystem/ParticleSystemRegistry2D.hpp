//
// ParticleSystemUpdater2D.h
// Author: Antoine Bastide
// Date: 16/01/2025
//

#ifndef PARTICLE_SYSTEM_UPDATER2D_H
#define PARTICLE_SYSTEM_UPDATER2D_H

#include <unordered_set>
#include <vector>

#include "Engine2D/ParticleSystem/ParticleSystem2D.hpp"

namespace Engine2D {
  namespace Rendering {
    class Renderer2D;
  }

  class ParticleSystem2D;
  class ParticleSystemRegistry2D {
    friend class Entity2D;
    friend class Game2D;
    friend class ParticleSystem2D;
    friend class Rendering::Renderer2D;

    /// All the particle systems in the game
    static std::vector<std::shared_ptr<ParticleSystem2D>> particleSystems;

    /// The particle systems that are pending to be added but do not have a sprite or a texture
    static std::vector<std::shared_ptr<ParticleSystem2D>> invalidParticleSystems;
    /// The particle systems pending to be added
    static std::vector<std::shared_ptr<ParticleSystem2D>> particleSystemsToAdd;
    /// The particle systems pending to be removed
    static std::unordered_set<std::shared_ptr<ParticleSystem2D>> particleSystemsToRemove;

    /// Whether we need to recompute the subranges
    static bool repartition;
    /// Whether a particle system has changed sprites or render order
    static bool dirty;
    /// The separation between transparent and opaque
    static std::ranges::borrowed_subrange_t<std::vector<std::shared_ptr<ParticleSystem2D>> &> subrange;

    /// Creates the appropriate subrange needed based on the given data and renders it
    static void prerender();

    /// Adds the given particle system so that it can be renderer
    static void addParticleSystem(const std::shared_ptr<ParticleSystem2D> &particleSystem);
    /// Removes the given particle system so that it can no longer be renderer
    static void removeParticleSystem(const std::shared_ptr<ParticleSystem2D> &particleSystem);
  };
} // Engine2D

#endif //PARTICLE_SYSTEM_UPDATER2D_H
