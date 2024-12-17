//
// Settings.h
// Author: Antoine Bastide
// Date: 16/12/2024
//

#ifndef SETTINGS_H
#define SETTINGS_H

#include "2D/Types/Vector2.h"

namespace Engine {
  /// Static class containing all the settings related to the engine
  class Settings {
    public:
      /// Static class containing all the settings related to the window
      class WindowSettings {
        friend class Settings;
        public:
          /// Changes the current screen resolution to the new value
          static void SetScreenResolution(Engine2D::Vector2<size_t> newValue);
          /// If true, the window will use the name of the scene for it's title
          static void SetUseSceneNameForTitle(bool newState);
          /// Changes the title of the window
          static void SetTitle(const std::string &newTitle);
          /// If true, allows window resizing
          static void SetAllowResize(bool newState);

          /// @returns The initial screen resolution (before any resizing)
          [[nodiscard]] static Engine2D::Vector2<size_t> GetScreenResolution();
          /// @returns True if the window will use the name of the scene for it's title, false if not
          [[nodiscard]] static bool GetUseSceneNameForTitle();
          /// @returns The title of the window
          [[nodiscard]] static std::string GetTitle();
          /// @returns True if the user can resize the window, false if not
          [[nodiscard]] static bool GetAllowResize();
        private:
          static Engine2D::Vector2<size_t> resolution;
          static bool useSceneNameForTitle;
          static std::string title;
          static bool allowResize;

          WindowSettings() = default;
      };

      /// Static class containing all the settings related to the graphics
      class GraphicsSettings {
        friend class Settings;
        public:
          /// If true, the render rate will match the refresh rate of the current monitor the screen is running on
          static void SetVsyncEnabled(bool newState);
          /// If true, the viewport will maintain the aspect ratio defined by the initial screen resolution
          static void SetMaintainAspectRatio(bool newState);
          /// Changes the frame rate will target, if set to 0, the game will not cap the frame rate.
          static void SetTargetFrameRate(unsigned int newValue);

          [[nodiscard]] static bool GetVsyncEnabled();
          [[nodiscard]] static bool GetMaintainAspectRatio();
          [[nodiscard]] static unsigned int GetTargetFrameRate();
        private:
          static bool vsyncEnabled;
          static bool maintainAspectRatio;
          static unsigned int targetFrameRate;

          GraphicsSettings() = default;
      };

      /// Static class containing all the settings related to the physics engine
      class PhysicsSettings {
        friend class Settings;
        public:
          /// Changes the rate at which the physics simulations are run, clamped between 0.02f and 0.33f (50 or 60 times a second).
          static void SetFixedDeltaTime(float newValue);
          /// Changes if the physics engine should use spacial partitioning to resolve collisions or not.
          /// It does not always lead to a performance boost, consider using it if you have a lot of dynamic colliders on screen.
          static void SetUseScreenPartitioning(bool newState);
          /// Changes the size of the partitioning of the screen for the physics simulation, defaulted to 4x4.
          /// Increasing the value does not always lead to a performance boost.
          /// It is better to test different values to find the best size for your game.
          static void SetPartitionSize(Engine2D::Vector2<size_t> newValue);
          /// Changes the value of the gravity used in the physics simulation.
          static void SetGravity(Engine2D::Vector2f newValue);

          /// @returns The rate at which the physics simulations are run
          [[nodiscard]] static float GetFixedDeltaTime();
          /// @returns True if the physics engine is currently using screen partitioning, false if not
          [[nodiscard]] static bool GetUseScreenPartitioning();
          /// @return The size of the screen partitioning used by the physics engine
          [[nodiscard]] static Engine2D::Vector2<size_t> GetPartitionSize();
          /// @returns The value of gravity used by the physics engine
          [[nodiscard]] static Engine2D::Vector2f GetGravity();
        private:
          static float fixedDeltaTime;
          static bool useScreenPartitioning;
          static Engine2D::Vector2<size_t> partitionSize;
          static Engine2D::Vector2f gravity;

          PhysicsSettings() = default;
      };

      /// All the settings related to the window
      static WindowSettings Window;
      /// All the settings related to the graphics
      static GraphicsSettings Graphics;
      /// All the settings related to the physics engine
      static PhysicsSettings Physics;
    private:
      Settings() = default;
  };
}

#endif //SETTINGS_H
