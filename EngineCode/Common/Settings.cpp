//
// Settings.cpp
// Author: Antoine Bastide
// Date: 16/12/2024
//

#include "Common/Settings.h"
#include "Engine2D/Game2D.h"
#include "Engine2D/Physics/Physics2D.h"
#include "Common/RenderingHeaders.h"

namespace Engine {
  glm::vec<2, size_t> Settings::Window::resolution = glm::vec<2, size_t>(800, 600);
  std::string Settings::Window::title = "Game Window";
  bool Settings::Window::allowResize = true;

  void Settings::Window::SetScreenResolution(const glm::vec<2, size_t> newValue) {
    resolution = newValue;
    glfwSetWindowSize(Engine2D::Game2D::instance->window, newValue.x, newValue.y);
  }

  void Settings::Window::SetTitle(const std::string &newTitle) {
    title = std::move(newTitle);
    glfwSetWindowTitle(Engine2D::Game2D::instance->window, title.c_str());
  }

  void Settings::Window::SetAllowResize(const bool newState) {
    allowResize = newState;
  }

  glm::vec<2, size_t> Settings::Window::GetScreenResolution() {
    return resolution;
  }

  std::string Settings::Window::GetTitle() {
    return title;
  }

  bool Settings::Window::GetAllowResize() {
    return allowResize;
  }

  bool Settings::Graphics::vsyncEnabled = false;
  bool Settings::Graphics::maintainAspectRatio = true;
  unsigned int Settings::Graphics::targetFrameRate = 0;

  void Settings::Graphics::SetVsyncEnabled(const bool newState) {
    vsyncEnabled = newState;
    glfwSwapInterval(vsyncEnabled);

    if (vsyncEnabled)
      Engine2D::Game2D::instance->targetFrameRate = 0;
    else if (targetFrameRate > 0) {
      Engine2D::Game2D::instance->targetFrameRate = targetFrameRate;
      Engine2D::Game2D::instance->targetRenderRate = 1.0f / targetFrameRate;
    }
  }

  void Settings::Graphics::SetMaintainAspectRatio(const bool newState) {
    maintainAspectRatio = newState;
  }

  void Settings::Graphics::SetTargetFrameRate(const unsigned int newValue) {
    targetFrameRate = newValue;

    if (targetFrameRate > 0) {
      Engine2D::Game2D::instance->targetFrameRate = targetFrameRate;
      Engine2D::Game2D::instance->targetRenderRate = 1.0f / targetFrameRate;
    } else {
      Engine2D::Game2D::instance->targetFrameRate = 0;
      Engine2D::Game2D::instance->targetRenderRate = 0;
    }
  }

  bool Settings::Graphics::GetVsyncEnabled() {
    return vsyncEnabled;
  }

  bool Settings::Graphics::GetMaintainAspectRatio() {
    return maintainAspectRatio;
  }

  unsigned int Settings::Graphics::GetTargetFrameRate() {
    return targetFrameRate;
  }

  float Settings::Physics::fixedDeltaTime = 0.02f;
  bool Settings::Physics::frictionEnabled = true;
  bool Settings::Physics::useScreenPartitioning = false;
  glm::vec<2, size_t> Settings::Physics::partitionSize = glm::vec<2, size_t>(4, 4);
  glm::vec2 Settings::Physics::gravity = glm::vec2(0.0f, -9.81f);

  void Settings::Physics::SetFixedDeltaTime(const float newValue) {
    constexpr float min = 1.0f / 50.0f;
    constexpr float max = 1.0f / 60.0f;
    fixedDeltaTime = std::clamp(newValue, min, max);
  }

  void Settings::Physics::SetFrictionEnabled(const bool newState) {
    frictionEnabled = newState;
  }

  void Settings::Physics::SetUseScreenPartitioning(const bool newState) {
    useScreenPartitioning = newState;
  }

  void Settings::Physics::SetPartitionSize(const glm::vec<2, size_t> newValue) {
    partitionSize = newValue;
    Engine2D::Game2D::instance->physics2D->collisionGridNeedsResizing = true;
  }

  void Settings::Physics::SetGravity(const glm::vec2 newValue) {
    gravity = newValue;
  }

  float Settings::Physics::GetFixedDeltaTime() {
    return fixedDeltaTime;
  }

  float Settings::Physics::GetFrictionEnabled() {
    return frictionEnabled;
  }

  bool Settings::Physics::GetUseScreenPartitioning() {
    return useScreenPartitioning;
  }

  glm::vec<2, size_t> Settings::Physics::GetPartitionSize() {
    return partitionSize;
  }

  glm::vec2 Settings::Physics::GetGravity() {
    return gravity;
  }

  bool Settings::Input::allowMouseInput = true;
  bool Settings::Input::allowKeyboardInput = true;
  bool Settings::Input::allowGamepadInput = true;
  float Settings::Input::gamepadStickThreshold = 0.15f;
  float Settings::Input::gamepadTriggerThreshold = -1.0f;

  void Settings::Input::SetAllowMouseInput(const bool newState) {
    allowMouseInput = newState;
  }

  void Settings::Input::SetAllowKeyboardInput(const bool newState) {
    allowKeyboardInput = newState;
  }

  void Settings::Input::SetAllowGamepadInput(const bool newState) {
    allowGamepadInput = newState;
  }

  void Settings::Input::SetGamepadStickThreshold(const float newValue) {
    gamepadStickThreshold = std::clamp(newValue, -1.0f, 1.0f);
  }

  void Settings::Input::SetGamepadTriggerThreshold(const float newValue) {
    gamepadTriggerThreshold = std::clamp(newValue, -1.0f, 1.0f);
  }

  bool Settings::Input::GetAllowMouseInput() {
    return allowMouseInput;
  }

  bool Settings::Input::GetAllowKeyboardInput() {
    return allowKeyboardInput;
  }

  bool Settings::Input::GetAllowGamepadInput() {
    return allowGamepadInput;
  }

  float Settings::Input::GetGamepadStickThreshold() {
    return gamepadStickThreshold;
  }

  float Settings::Input::GetGamepadTriggerThreshold() {
    return gamepadTriggerThreshold;
  }
}
