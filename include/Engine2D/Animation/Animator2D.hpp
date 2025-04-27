//
// Animator.hpp
// Author: Antoine Bastide
// Date: 25/04/2025
//

#ifndef ANIMATOR2D_HPP
#define ANIMATOR2D_HPP

#include <string_view>
#include <unordered_map>

#include "Engine2D/Component2D.hpp"
#include "Engine2D/Animation/Animation2D.hpp"

namespace Engine2D::Animation {
  class Animator2D : public Component2D {
    friend class AnimationSystem;
    public:
      /// Maps the given animation to it's name in the animator
      /// @note If an animation with the same name already exists, the new one will overwrite it
      void AddAnimation(std::string_view name, const Animation2D &animation);
      /// Removes the animation with the given name
      void RemoveAnimation(std::string_view name);
      /// Play's the animation with the given name
      void Play(std::string_view name);
      /// @returns A pointer to the animation with the given name, or nullptr if no animation has that name
      [[nodiscard]] Animation2D *GetAnimation(std::string_view name);
      /// @returns A pointer to the animation currently played by the animator
      [[nodiscard]] Animation2D *GetCurrentAnimation() const;
    private:
      /// The animations that this animator can play
      std::unordered_map<std::string_view, Animation2D> animations;
      /// The animation currently played by the animator
      Animation2D *currentAnimation;
      /// Updates the current animation and transitions
      void update();
  };
}

#endif //ANIMATOR2D_HPP
