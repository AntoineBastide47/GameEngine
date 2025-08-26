//
// Animation.hpp
// Author: Antoine Bastide
// Date: 25/04/2025
//

#ifndef ANIMATION2D_HPP
#define ANIMATION2D_HPP

#include <string>

#include "Engine/Reflection/Reflectable.hpp"
#include "Animation2D.gen.hpp"

namespace Engine2D::Animation {
  class Animation2D final : public Engine::Reflection::Reflectable {
    SERIALIZE_ANIMATION2D
      friend class Animator2D;
    public:
      /// The number of sprites in a vertical animation
      int frameCountX;
      /// The number of sprites in a horizontal animation
      int frameCountY;
      /// The offset at which to start the animation on the x-axis
      int frameOffsetX;
      /// The offset at which to start the animation on the y-axis
      int frameOffsetY;
      /// The current frame of the animation
      int currentFrame;
      /// Whether the animation will play again once it finishes
      bool loop;
      /// Whether to play the animation in reverse
      bool reverse;

      /// @warning Do not use, use the static factory methods Create* instead
      Animation2D();

      /**
       * Creates a vertical animation from a vertical strip image
       * @param spriteName The name of the sprite this animation is based on
       * @param frameCount The number of frames in the sprite
       * @param frameOffsetX Frame start index offset on the x-axis
       * @param frameOffsetY Frame start index offset on the y-axis
       * @param frameRate The time between each frame change
       * @param loop If the animation should loop
       * @param reverse If the animation should play in reverse
       */
      static Animation2D CreateVertical(
        const std::string &spriteName, int frameCount, int frameOffsetX, int frameOffsetY, float frameRate, bool loop,
        bool reverse
      );

      /**
       * Creates a horizontal animation from a horizontal strip image
       * @param spriteName The name of the sprite this animation is based on
       * @param frameCount The number of frames in the sprite
       * @param frameOffsetX Frame start index offset on the x-axis
       * @param frameOffsetY Frame start index offset on the y-axis
       * @param frameRate The time between each frame change
       * @param loop If the animation should loop
       * @param reverse If the animation should play in reverse
       */
      static Animation2D CreateHorizontal(
        const std::string &spriteName, int frameCount, int frameOffsetX, int frameOffsetY, float frameRate, bool loop,
        bool reverse
      );

      /// Sets the duration of each frame in the animation
      void SetFrameDuration(float frameDuration);
      /// @returns The duration of each frame in the animation
      [[nodiscard]] float FrameDuration() const;
      /// Sets the speed of the animation, set to 1 by default
      void SetSpeed(float speed);
      /// @returns The speed of the animation
      [[nodiscard]] float Speed() const;

      void OnDeserialize(Engine::Reflection::Format format, const Engine::JSON &json) override;
    private:
      /// The name of the sprite used by this animation
      ENGINE_SERIALIZE std::string spriteName;
      /// True if vertical, False if horizontal
      ENGINE_SERIALIZE bool vertical;
      /// Whether the animation is playing or paused
      ENGINE_SERIALIZE bool paused;
      /// Whether the animation has finished playing
      ENGINE_SERIALIZE bool completed;
      /// The duration of each frame
      ENGINE_SERIALIZE float frameDuration;
      /// 1.0f / frameDuration
      float frameDurationInv;
      /// The speed of the animation
      ENGINE_SERIALIZE float speed;
      /// 1.0f / speed
      float speedInv;
      /// How long the animation has been running for
      float elapsedTime;

      Animation2D(
        const std::string &spriteName, int frameCountX, int frameCountY, int frameOffsetX, int frameOffsetY,
        float frameRate,
        bool vertical, bool loop, bool reverse
      );

      int FrameCount() const;

      void OnEditorValueChanged() override;
  };
}

#endif //ANIMATION2D_HPP
