//
// AnimationSystem.hpp
// Author: Antoine Bastide
// Date: 25/04/2025
//

#ifndef ANIMATION_SYSTEM_HPP
#define ANIMATION_SYSTEM_HPP
#include <vector>

namespace Engine2D {
  class Game2D;
  class Entity2D;
}

namespace Engine2D::Animation {
  class Animator2D;

  class AnimationSystem {
    friend class Animator2D;
    friend class Engine2D::Game2D;

    inline static std::vector<Animator2D *> animators;

    static void addAnimator(Animator2D *animator);
    static void removeAnimator(Animator2D *animator);
    static void update();
  };
}

#endif //ANIMATION_SYSTEM_HPP
