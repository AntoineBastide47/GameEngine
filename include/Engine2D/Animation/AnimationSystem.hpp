//
// AnimationSystem.hpp
// Author: Antoine Bastide
// Date: 25/04/2025
//

#ifndef ANIMATION_SYSTEM_HPP
#define ANIMATION_SYSTEM_HPP

#include <vector>

namespace Engine2D {
  class Scene;
}

namespace Engine2D::Animation {
  class Animator2D;

  class AnimationSystem {
    friend class Animator2D;
    friend class Engine2D::Scene;

    std::vector<Animator2D *> animators;

    void addAnimator(Animator2D *animator);
    void removeAnimator(Animator2D *animator);
    void update() const;
  };
}

#endif //ANIMATION_SYSTEM_HPP
