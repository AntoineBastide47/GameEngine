//
// ParticleSystemRenderer2D.cpp
// Author: Antoine Bastide
// Date: 16/01/2025
//

#include "Engine/RenderingHeaders.hpp"
#include "Engine2D/ParticleSystem/ParticleSystemRenderer2D.hpp"

#include <iostream>

#include "Engine2D/ParticleSystem/ParticleSystem2D.hpp"
#include "Engine/Macros/Profiling.hpp"
#include "Engine2D/Entity2D.hpp"
#include "Engine2D/Rendering/Sprite.hpp"

namespace Engine2D {
  std::vector<std::shared_ptr<ParticleSystem2D>> ParticleSystemRenderer2D::particleSystems;
  std::vector<std::shared_ptr<ParticleSystem2D>> ParticleSystemRenderer2D::particleSystemsToAdd;
  std::unordered_set<std::shared_ptr<ParticleSystem2D>> ParticleSystemRenderer2D::particleSystemsToRemove;
  bool ParticleSystemRenderer2D::repartition;
  bool ParticleSystemRenderer2D::dirty;
  std::ranges::borrowed_subrange_t<std::vector<std::shared_ptr<ParticleSystem2D>> &> ParticleSystemRenderer2D::subrange;
  std::ranges::borrowed_subrange_t<std::vector<std::shared_ptr<ParticleSystem2D>> &>
  ParticleSystemRenderer2D::subrangeTransparentInFront;
  #if MULTI_THREAD
  std::mutex ParticleSystemRenderer2D::particleSystemMutex;
  #endif

  void ParticleSystemRenderer2D::update() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    #if MULTI_THREAD
    std::lock_guard lock(particleSystemMutex);
    #endif

    const size_t lastParticleCount = particleSystems.size();

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

    if (particleSystems.empty())
      return;

    repartition = lastParticleCount != particleSystems.size();

    for (const auto &particleSystem: particleSystems)
      particleSystem->update();
  }

  void ParticleSystemRenderer2D::render(const bool transparent, const ParticleSystem2D::RenderOrderType order) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    #if MULTI_THREAD
    std::lock_guard lock(particleSystemMutex);
    #endif

    if (particleSystems.empty())
      return;

    if (repartition || dirty) {
      subrange = std::ranges::stable_partition(
        particleSystems, [&](const auto &p) {
          return p->sprite->transparent;
        }
      );
      subrangeTransparentInFront = std::ranges::stable_partition(
        particleSystems.begin(), subrange.begin(), [&](const auto &p) {
          return p->renderOrderType == ParticleSystem2D::RenderOrderType::InFrontOfSprite;
        }
      );
    }

    dirty = false;

    // Transparent particles
    if (transparent) {
      auto [inFrontBegin, inFrontEnd] = subrangeTransparentInFront;

      // Render the transparent sprites that are in front of the main sprite
      if (order == ParticleSystem2D::RenderOrderType::InFrontOfSprite) {
        if (const auto inFrontRange = std::ranges::subrange(particleSystems.begin(), inFrontBegin); !inFrontRange.empty()) {
          glBlendFunc(GL_SRC_ALPHA, GL_ONE);
          for (const auto &particleSystem: inFrontRange)
            particleSystem->render();
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
      }
      // Render the transparent sprites that are behind the main sprite
      else if (order == ParticleSystem2D::RenderOrderType::BehindSprite) {
        if (const auto behindRange = std::ranges::subrange(inFrontBegin, inFrontEnd); !behindRange.empty()) {
          glBlendFunc(GL_SRC_ALPHA, GL_ONE);
          for (const auto &particleSystem: behindRange)
            particleSystem->render();
          glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        }
      }
    }
    // Opaque particles
    else if (!subrange.empty())
      for (const auto &particleSystem: subrange)
        particleSystem->render();
  }

  void ParticleSystemRenderer2D::addParticleSystem(const std::shared_ptr<ParticleSystem2D> &particleSystem) {
    particleSystemsToAdd.emplace_back(particleSystem);
  }

  void ParticleSystemRenderer2D::removeParticleSystem(const std::shared_ptr<ParticleSystem2D> &particleSystem) {
    particleSystemsToRemove.insert(particleSystem);
  }
} // Engine2D
