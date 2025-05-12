//
// Animator.cpp
// Author: Antoine Bastide
// Date: 25/04/2025
//

#include "Engine2D/Animation/Animator2D.hpp"
#include "Engine/Macros/Assert.hpp"
#include "Engine2D/Entity2D.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/Rendering/Sprite.hpp"
#include "Engine2D/Rendering/SpriteRenderer.hpp"

namespace Engine2D::Animation {
  void Animator2D::AddAnimation(const std::string_view name, const Animation2D &animation) {
    if (const auto it = animations.find(name); it != animations.end())
      it->second = animation;
    else
      animations.emplace(name, animation);
  }

  void Animator2D::RemoveAnimation(const std::string_view name) {
    animations.erase(name);
  }

  void Animator2D::Play(const std::string_view name) {
    if (animations.contains(name)) {
      Animation2D &animation = animations.at(name);
      animation.Restart();
      currentAnimation = &animation;
    }
  }

  Animation2D *Animator2D::GetAnimation(const std::string_view name) {
    if (const auto it = animations.find(name); it != animations.end())
      return &it->second;
    return nullptr;
  }

  Animation2D *Animator2D::GetCurrentAnimation() const {
    return currentAnimation;
  }

  void Animator2D::update() {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSubSystem);

    if (!currentAnimation)
      return;

    const auto renderer = Entity()->GetComponent<Rendering::SpriteRenderer>();
    if (!renderer || animations.empty())
      return;

    const auto sprite = renderer->GetSprite();
    if (!sprite)
      return;

    auto &rect = sprite->rect;
    if (Animation2D &animation = *currentAnimation; !animation.paused && !animation.completed) {
      // Set the current frame of the animation
      const int frameCount = animation.vertical ? animation.frameCountY : animation.frameCountX;
      animation.elapsedTime += Game2D::DeltaTime();
      const float totalDuration = frameCount * animation.frameDuration;

      const int frame = std::floor(std::fmod(animation.elapsedTime, totalDuration) * animation.inverseFrameDuration);
      if (animation.reverse)
        animation.currentFrame = frameCount - 1 - frame;
      else
        animation.currentFrame = frame;

      // When at the end of the animation, check if it needs to restart
      if (animation.elapsedTime >= totalDuration) {
        animation.Restart();
        if (!animation.loop)
          animation.completed = true;
      }

      // Update the rect of the sprite to reflect the animation
      if (animation.vertical) {
        rect.y = (animation.currentFrame + animation.frameOffsetY) * rect.w;
        rect.x = animation.frameOffsetX * rect.z;
      } else {
        rect.x = (animation.currentFrame + animation.frameOffsetX) * rect.z;
        rect.y = animation.frameOffsetY * rect.w;
      }
    }
  }
}
