//
// AnimationSystem.cpp
// Author: Antoine Bastide
// Date: 25/04/2025
//

#include "Engine2D/Animation/AnimationSystem.hpp"

#include "Engine/Macros/Profiling.hpp"
#include "Engine2D/Animation/Animator2D.hpp"

namespace Engine2D::Animation {
  std::vector<std::shared_ptr<Animator2D>> AnimationSystem::animators;

  void AnimationSystem::addAnimator(const std::shared_ptr<Animator2D> &animator) {
    if (animator)
      animators.emplace_back(animator);
  }

  void AnimationSystem::removeAnimator(const std::shared_ptr<Animator2D> &animator) {
    if (!animators.empty() && animator)
      std::erase(animators, animator);
  }

  void AnimationSystem::update() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);
    for (const auto& animator : animators)
      animator->update();
  }
}
