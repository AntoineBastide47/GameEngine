//
// Animation.cpp
// Author: Antoine Bastide
// Date: 25/04/2025
//

#include "Engine2D/Animation/Animation2D.hpp"

namespace Engine2D::Animation {
  Animation2D::Animation2D(
    const std::string &spriteName, const int frameCountX, const int frameCountY, const int frameOffsetX, const int frameOffsetY,
    const float frameRate, const bool vertical, const bool loop, const bool reverse
  )
    : frameCountX(frameCountX), frameCountY(frameCountY), frameOffsetX(frameOffsetX), frameOffsetY(frameOffsetY),
      currentFrame(0), loop(loop), reverse(reverse), vertical(vertical), paused(false), completed(false),
      frameDuration(frameRate), inverseFrameDuration(1.0f / frameRate), speed(1), inverseSpeed(1), elapsedTime(0),
      spriteName(std::move(spriteName)) {}

  int Animation2D::FrameCount() const {
    return vertical ? frameCountY : frameCountX;
  }

  Animation2D Animation2D::CreateVertical(
    const std::string &spriteName, const int frameCount, const int frameOffsetX, const int frameOffsetY, const float frameRate,
    const bool loop, const bool reverse
  ) {
    return Animation2D{spriteName, 0, frameCount, frameOffsetX, frameOffsetY, frameRate, true, loop, reverse};
  }

  Animation2D Animation2D::CreateHorizontal(
    const std::string &spriteName, const int frameCount, const int frameOffsetX, const int frameOffsetY, const float frameRate,
    const bool loop, const bool reverse
  ) {
    return Animation2D{spriteName, frameCount, 0, frameOffsetX, frameOffsetY, frameRate, false, loop, reverse};
  }

  void Animation2D::SetFrameDuration(const float frameDuration) {
    this->frameDuration = frameDuration;
    inverseFrameDuration = 1.0f / frameDuration;
  }

  float Animation2D::GetFrameDuration() const {
    return frameDuration;
  }

  void Animation2D::SetSpeed(const float speed) {
    this->speed = speed;
    inverseSpeed = 1.0f / speed;
  }

  float Animation2D::GetSpeed() const {
    return speed;
  }
}
