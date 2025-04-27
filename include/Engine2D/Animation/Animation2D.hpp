//
// Animation.hpp
// Author: Antoine Bastide
// Date: 25/04/2025
//

#ifndef ANIMATION2D_HPP
#define ANIMATION2D_HPP

namespace Engine2D::Animation {
  class Animation2D {
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

      /// Creates a vertical animation from a vertical strip image
      static Animation2D CreateVertical(
        int frameCount, int frameOffsetX, int frameOffsetY, float frameRate, bool loop, bool reverse
      );
      /// Creates a horizontal animation from a horizontal strip image
      static Animation2D CreateHorizontal(
        int frameCount, int frameOffsetX, int frameOffsetY, float frameRate, bool loop, bool reverse
      );

      /// Sets the duration of each frame in the animation
      void SetFrameDuration(float frameDuration);
      /// @returns The duration of each frame in the animation
      [[nodiscard]] float GetFrameDuration() const;
      /// Sets the speed of the animation, set to 1 by default
      void SetSpeed(float speed);
      /// @returns The speed of the animation
      [[nodiscard]] float GetSpeed() const;

      /// Stops the animation from continuing to play
      void Pause();
      /// Allows the animation to play once again
      void Resume();
      /// Restarts the animation if it is non-looping
      void Restart();
    private:
      /// True if vertical, False if horizontal
      bool vertical;
      /// Whether the animation is playing or paused
      bool paused;
      /// Whether the animation has finished playing
      bool completed;
      /// The duration of each frame
      float frameDuration;
      /// 1.0f / frameDuration
      float inverseFrameDuration;
      /// The speed of the animation
      float speed;
      /// 1.0f / speed
      float inverseSpeed;
      /// How long the animation has been running for
      float elapsedTime;

      Animation2D(
        int frameCountX, int frameCountY, int frameOffsetX, int frameOffsetY, float frameRate, bool vertical, bool loop,
        bool reverse
      );
  };
}

#endif //ANIMATION2D_HPP
