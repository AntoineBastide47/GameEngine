//
// Settings.cpp
// Author: Antoine Bastide
// Date: 16/12/2024
//

#include "Settings.h"
#include "2D/Game2D.h"
#include "Common/RenderingHeaders.h"

namespace Engine {
  Engine2D::Vector2<size_t> Settings::WindowSettings::resolution = Engine2D::Vector2<size_t>{800, 600};
  bool Settings::WindowSettings::useSceneNameForTitle = false;
  std::string Settings::WindowSettings::title = "Game Window";
  bool Settings::WindowSettings::allowResize = true;

  bool Settings::GraphicsSettings::vsyncEnabled = false;
  bool Settings::GraphicsSettings::maintainAspectRatio = true;
  unsigned int Settings::GraphicsSettings::targetFrameRate = 0;

  float Settings::PhysicsSettings::fixedDeltaTime = 0.02f;
  bool Settings::PhysicsSettings::useScreenPartitioning = false;
  Engine2D::Vector2<size_t> Settings::PhysicsSettings::partitionSize = Engine2D::Vector2<size_t>{4, 4};
  Engine2D::Vector2f Settings::PhysicsSettings::gravity = Engine2D::Vector2f(0.0f, -9.81f);

  void Settings::WindowSettings::SetScreenResolution(const Engine2D::Vector2<size_t> newValue) {
    resolution = newValue;
    glfwSetWindowSize(Engine2D::Game2D::instance->window, newValue.x, newValue.y);
  }

  void Settings::WindowSettings::SetUseSceneNameForTitle(const bool newState) {
    useSceneNameForTitle = newState;
  }

  void Settings::WindowSettings::SetTitle(const std::string &newTitle) {
    title = std::move(newTitle);
    glfwSetWindowTitle(Engine2D::Game2D::instance->window, title.c_str());
  }

  void Settings::WindowSettings::SetAllowResize(const bool newState) {
    allowResize = newState;
  }

  void Settings::GraphicsSettings::SetVsyncEnabled(const bool newState) {
    vsyncEnabled = newState;
    glfwSwapInterval(vsyncEnabled);

    if (vsyncEnabled)
      Engine2D::Game2D::instance->targetFrameRate = 0;
    else if (targetFrameRate > 0) {
      Engine2D::Game2D::instance->targetFrameRate = targetFrameRate;
      Engine2D::Game2D::instance->targetRenderRate = 1.0f / targetFrameRate;
    }
  }

  void Settings::GraphicsSettings::SetMaintainAspectRatio(const bool newState) {
    maintainAspectRatio = newState;
  }

  void Settings::GraphicsSettings::SetTargetFrameRate(const unsigned int newValue) {
    targetFrameRate = newValue;

    if (targetFrameRate > 0) {
      Engine2D::Game2D::instance->targetFrameRate = targetFrameRate;
      Engine2D::Game2D::instance->targetRenderRate = 1.0f / targetFrameRate;
    }
  }

  void Settings::PhysicsSettings::SetFixedDeltaTime(const float newValue) {
    constexpr float min = 1.0f / 50.0f;
    constexpr float max = 1.0f / 60.0f;
    fixedDeltaTime = std::clamp(newValue, min, max);
  }

  void Settings::PhysicsSettings::SetUseScreenPartitioning(const bool newState) {
    useScreenPartitioning = newState;
  }

  void Settings::PhysicsSettings::SetPartitionSize(const Engine2D::Vector2<size_t> newValue) {
    partitionSize = newValue;
    Engine2D::Game2D::instance->physics2D->collisionGridNeedsResizing = true;
  }

  void Settings::PhysicsSettings::SetGravity(const Engine2D::Vector2f newValue) {
    gravity = newValue;
  }

  Engine2D::Vector2<size_t> Settings::WindowSettings::GetScreenResolution() {
    return resolution;
  }

  bool Settings::WindowSettings::GetUseSceneNameForTitle() {
    return useSceneNameForTitle;
  }

  std::string Settings::WindowSettings::GetTitle() {
    return title;
  }

  bool Settings::WindowSettings::GetAllowResize() {
    return allowResize;
  }

  bool Settings::GraphicsSettings::GetVsyncEnabled() {
    return vsyncEnabled;
  }

  bool Settings::GraphicsSettings::GetMaintainAspectRatio() {
    return maintainAspectRatio;
  }

  unsigned int Settings::GraphicsSettings::GetTargetFrameRate() {
    return targetFrameRate;
  }

  float Settings::PhysicsSettings::GetFixedDeltaTime() {
    return fixedDeltaTime;
  }

  bool Settings::PhysicsSettings::GetUseScreenPartitioning() {
    return useScreenPartitioning;
  }

  Engine2D::Vector2<size_t> Settings::PhysicsSettings::GetPartitionSize() {
    return partitionSize;
  }

  Engine2D::Vector2f Settings::PhysicsSettings::GetGravity() {
    return gravity;
  }
}
