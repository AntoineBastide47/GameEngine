//
// Settings.cpp
// Author: Antoine Bastide
// Date: 16/12/2024
//

#include "Engine/Settings.hpp"
#include "Engine/RenderingHeaders.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/Physics/Physics2D.hpp"
#include "Engine2D/SceneManagement/SceneManager.hpp"

namespace Engine {
  void Settings::Window::SetScreenResolution(const glm::vec<2, size_t> newValue) {
    if (!Engine2D::Game2D::instance)
      return;
    resolution = newValue;
    if (!Engine2D::Game2D::instance->IsHeadless()) {
      glfwSetWindowSize(Engine2D::Game2D::instance->window, newValue.x, newValue.y);
    }
  }

  void Settings::Window::SetTitle(const std::string &newTitle) {
    if (!Engine2D::Game2D::instance)
      return;
    title = std::move(newTitle);
    if (!Engine2D::Game2D::instance->IsHeadless()) {
      glfwSetWindowTitle(Engine2D::Game2D::instance->window, title.c_str());
    }
  }

  void Settings::Window::SetAllowResize(const bool newState) {
    allowResize = newState;
  }

  glm::vec<2, size_t> Settings::Window::ScreenResolution() {
    return resolution;
  }

  std::string Settings::Window::Title() {
    return title;
  }

  bool Settings::Window::AllowResize() {
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

  bool Settings::Graphics::VsyncEnabled() {
    return vsyncEnabled;
  }

  bool Settings::Graphics::MaintainAspectRatio() {
    return maintainAspectRatio;
  }

  unsigned int Settings::Graphics::TargetFrameRate() {
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
    Engine2D::SceneManager::ActiveScene()->physicsSystem.collisionGridNeedsResizing = true;
  }

  void Settings::Physics::SetGravity(const glm::vec2 newValue) {
    gravity = newValue;
  }

  float Settings::Physics::FixedDeltaTime() {
    return fixedDeltaTime;
  }

  bool Settings::Physics::UseScreenPartitioning() {
    return useScreenPartitioning;
  }

  glm::vec<2, size_t> Settings::Physics::PartitionSize() {
    return partitionSize;
  }

  glm::vec2 Settings::Physics::Gravity() {
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

  bool Settings::Input::AllowMouseInput() {
    return allowMouseInput;
  }

  bool Settings::Input::AllowKeyboardInput() {
    return allowKeyboardInput;
  }

  bool Settings::Input::AllowGamepadInput() {
    return allowGamepadInput;
  }

  float Settings::Input::GamepadStickThreshold() {
    return gamepadStickThreshold;
  }

  float Settings::Input::GamepadTriggerThreshold() {
    return gamepadTriggerThreshold;
  }

  void Settings::Profiling::SetProfilingLevel(const Level newValue) {
    profilingLevel = newValue;
  }

  Settings::Profiling::Level Settings::Profiling::ProfilingLevel() {
    return profilingLevel;
  }

  void Settings::Profiling::SetProfilingThreshold(const unsigned long long newValue) {
    profilingThreshold = newValue;
  }

  unsigned long long Settings::Profiling::ProfilingThreshold() {
    return profilingThreshold;
  }
}
