//
// ParticleSystemUpdater2D.h
// Author: Antoine Bastide
// Date: 16/01/2025
//

#ifndef PARTICLE_SYSTEM_UPDATER2D_H
#define PARTICLE_SYSTEM_UPDATER2D_H

#include <unordered_set>
#include <vector>

namespace Engine2D {
  class Scene;
  class ParticleSystem2D;

  namespace Rendering {
    class Renderer2D;
  }

  class ParticleSystemRegistry2D {
    friend class Scene;
    friend class ParticleSystem2D;
    friend class Rendering::Renderer2D;

    /// All the particle systems in the game
    std::vector<ParticleSystem2D *> particleSystems;

    /// The particle systems that are pending to be added but do not have a sprite or a texture
    std::vector<ParticleSystem2D *> invalidParticleSystems;
    /// The particle systems pending to be added
    std::vector<ParticleSystem2D *> particleSystemsToAdd;
    /// The particle systems pending to be removed
    std::unordered_set<ParticleSystem2D *> particleSystemsToRemove;
    /// The scene this registry is a part of
    Scene *scene;

    /// Whether we need to recompute the subranges
    bool repartition;
    /// Whether a particle system has changed sprites or render order
    bool dirty;
    /// The separation between transparent and opaque
    std::ranges::borrowed_subrange_t<std::vector<ParticleSystem2D *> &> subrange;

    ParticleSystemRegistry2D();

    /// Creates the appropriate subrange needed based on the given data and renders it
    void prerender();

    /// Adds the given particle system so that it can be renderer
    void addParticleSystem(ParticleSystem2D *particleSystem);
    /// Removes the given particle system so that it can no longer be renderer
    void removeParticleSystem(ParticleSystem2D *particleSystem);
  };
} // Engine2D

#endif //PARTICLE_SYSTEM_UPDATER2D_H
