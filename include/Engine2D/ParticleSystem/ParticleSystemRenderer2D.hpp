//
// ParticleSystemRenderer2D.h
// Author: Antoine Bastide
// Date: 16/01/2025
//

#ifndef PARTICLE_SYSTEM_RENDERER2D_H
#define PARTICLE_SYSTEM_RENDERER2D_H

#include <unordered_set>
#include <vector>

#include "Engine2D/ParticleSystem/ParticleSystem2D.hpp"

#if MULTI_THREAD
#include <mutex>
#endif

namespace Engine2D {
  class ParticleSystem2D;
  class ParticleSystemRenderer2D {
    friend class Entity2D;
    friend class Game2D;
    friend class ParticleSystem2D;

    /// All the particle systems in the game
    static std::vector<std::shared_ptr<ParticleSystem2D>> particleSystems;
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
    /// The separation between transparent in front and transparent behind
    static std::ranges::borrowed_subrange_t<std::vector<std::shared_ptr<ParticleSystem2D>> &> subrangeTransparentInFront;

    #if MULTI_THREAD
    static std::mutex particleSystemMutex;
    #endif

    /// Updates all particle systems
    static void update();
    /// Creates the appropriate subrange needed based on the given data and renders it
    static void render(bool transparent, ParticleSystem2D::RenderOrderType order);

    /// Adds the given particle system so that it can be renderer
    static void addParticleSystem(const std::shared_ptr<ParticleSystem2D> &particleSystem);
    /// Removes the given particle system so that it can no longer be renderer
    static void removeParticleSystem(const std::shared_ptr<ParticleSystem2D> &particleSystem);
  };
} // Engine2D

#endif //PARTICLE_SYSTEM_RENDERER2D_H
