//
// AnimationSystem.cpp
// Author: Antoine Bastide
// Date: 25/04/2025
//

#include "Engine2D/Animation/AnimationSystem.hpp"
#include "Engine/Macros/Profiling.hpp"
#include "Engine2D/Animation/Animator2D.hpp"

namespace Engine2D::Animation {
  void AnimationSystem::addAnimator(Animator2D *animator) {
    if (animator)
      animators.emplace_back(animator);
  }

  void AnimationSystem::removeAnimator(Animator2D *animator) {
    if (animator)
      std::erase(animators, animator);
  }

  void AnimationSystem::update() const {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerSystem);
    for (const auto &animator: animators)
      animator->update();
  }
}
