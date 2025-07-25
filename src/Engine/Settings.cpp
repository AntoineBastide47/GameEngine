//
// Settings.cpp
// Author: Antoine Bastide
// Date: 16/12/2024
//

#include "Engine/RenderingHeaders.hpp"
#include "Engine/Settings.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/Physics/Physics2D.hpp"

namespace Engine {
  void Settings::Window::SetScreenResolution(const glm::vec<2, size_t> newValue) {
    if (!Engine2D::Game2D::instance)
      return;
    resolution = newValue;
    glfwSetWindowSize(Engine2D::Game2D::instance->window, newValue.x, newValue.y);
  }

  void Settings::Window::SetTitle(const std::string &newTitle) {
    if (!Engine2D::Game2D::instance)
      return;
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

  void Settings::Graphics::SetVsyncEnabled(const bool newState) {
    vsyncEnabled = newState;
    glfwSwapInterval(vsyncEnabled ? 1 : 0);

    if (!vsyncEnabled && targetFrameRate > 0) {
      Engine2D::Game2D::instance->targetFrameRate = targetFrameRate;
      Engine2D::Game2D::instance->targetRenderRate = 1.0f / targetFrameRate;
    }
  }

  void Settings::Graphics::SetMaintainAspectRatio(const bool newState) {
    maintainAspectRatio = newState;
  }

  void Settings::Graphics::SetTargetFrameRate(const unsigned int newValue) {
    if (!Engine2D::Game2D::instance)
      return;

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

  void Settings::Physics::SetFixedDeltaTime(const float newValue) {
    static constexpr float min = 0.02f;
    static constexpr float max = 1.0f / 60.0f;
    fixedDeltaTime = std::clamp(newValue, min, max);
  }

  void Settings::Physics::SetUseScreenPartitioning(const bool newState) {
    useScreenPartitioning = newState;
  }

  void Settings::Physics::SetPartitionSize(const glm::vec<2, size_t> newValue) {
    partitionSize = newValue;
    Physics2D::collisionGridNeedsResizing = true;
  }

  void Settings::Physics::SetGravity(const glm::vec2 newValue) {
    gravity = newValue;
  }

  float Settings::Physics::GetFixedDeltaTime() {
    return fixedDeltaTime;
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

  Settings::Profiling::ProfilingLevel Settings::Profiling::profilingLevel = ProfilingLevel::Disabled;
  size_t Settings::Profiling::profilingThreshold = 50;

  void Settings::Profiling::SetProfilingLevel(const ProfilingLevel newValue) {
    profilingLevel = newValue;
  }

  Settings::Profiling::ProfilingLevel Settings::Profiling::GetProfilingLevel() {
    return profilingLevel;
  }

  void Settings::Profiling::SetProfilingThreshold(const size_t newValue) {
    profilingThreshold = newValue;
  }

  size_t Settings::Profiling::GetProfilingThreshold() {
    return profilingThreshold;
  }
}
