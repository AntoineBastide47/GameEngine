//
// Settings.h
// Author: Antoine Bastide
// Date: 16/12/2024
//

#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <glm/glm.hpp>

namespace Engine {
  /// Static class containing all the settings related to the engine
  class Settings {
    public:
      /// Static class containing all the settings related to the window
      class Window final {
        friend class Settings;
        public:
          /// Changes the current screen resolution to the new value
          static void SetScreenResolution(glm::vec<2, uint> newValue);
          /// Changes the title of the window
          static void SetTitle(const std::string &newTitle);
          /// If true, allows window resizing
          static void SetAllowResize(bool newState);

          /// @returns The initial screen resolution (before any resizing)
          [[nodiscard]] static glm::vec<2, uint> ScreenResolution();
          /// @returns The title of the window
          [[nodiscard]] static std::string Title();
          /// @returns True if the user can resize the window, false if not
          [[nodiscard]] static bool AllowResize();
        private:
          inline static auto resolution = glm::vec<2, uint>(800, 600);
          inline static std::string title = "Game Window";
          inline static bool allowResize = true;

          Window() = default;
      };

      /// Static class containing all the settings related to the graphics
      class Graphics final {
        friend class Settings;
        public:
          /// If true, the render rate will match the refresh rate of the current monitor the screen is running on
          static void SetVsyncEnabled(bool newState);
          /// If true, the viewport will maintain the aspect ratio defined by the initial screen resolution
          static void SetMaintainAspectRatio(bool newState);
          /// Changes the frame rate will target, if set to 0, the game will not cap the frame rate.<br>
          /// Warning: Uncapping the frame rate may cause the game to utilize maximum CPU resources in an attempt to achieve the highest possible frame rate.
          static void SetTargetFrameRate(unsigned int newValue);

          /// @returns True if the rendering will sync to the refresh rate of the monitor it is running on, false if not.
          [[nodiscard]] static bool VsyncEnabled();
          /// @returns True if the textures will scale with the window, false if not
          [[nodiscard]] static bool MaintainAspectRatio();
          /// @returns How many frames wil be rendered per second, if 0 the engine will render as many as possible
          [[nodiscard]] static unsigned int TargetFrameRate();
        private:
          inline static bool vsyncEnabled = false;
          inline static bool maintainAspectRatio = true;
          inline static unsigned int targetFrameRate = 0;

          Graphics() = default;
      };

      /// Static class containing all the settings related to the physics engine
      class Physics final {
        friend class Settings;
        public:
          /// Changes the rate at which the physics simulations are run, clamped between 0.02f and 0.016...7f (50 or 60 times a second).
          static void SetFixedDeltaTime(float newValue);
          /// Changes if the physics engine should use spacial partitioning to resolve collisions or not.
          /// It does not always lead to a performance boost, consider using it if you have a lot of dynamic colliders on screen.
          static void SetUseScreenPartitioning(bool newState);
          /// Changes the size of the partitioning of the screen for the physics simulation, defaulted to 4x4.
          /// Increasing the value does not always lead to a performance boost.
          /// It is better to test different values to find the best size for your game.
          static void SetPartitionSize(glm::vec<2, size_t> newValue);
          /// Changes the value of the gravity used in the physics simulation.
          static void SetGravity(glm::vec2 newValue);

          /// @returns The rate at which the physics simulations are run
          [[nodiscard]] static float FixedDeltaTime();
          /// @returns True if the physics engine is currently using screen partitioning, false if not
          [[nodiscard]] static bool UseScreenPartitioning();
          /// @return The size of the screen partitioning used by the physics engine
          [[nodiscard]] static glm::vec<2, size_t> PartitionSize();
          /// @returns The value of gravity used by the physics engine
          [[nodiscard]] static glm::vec2 Gravity();
        private:
          inline static float fixedDeltaTime = 1.0f / 60.0f;
          inline static bool useScreenPartitioning = false;
          inline static auto partitionSize = glm::vec<2, size_t>(4, 4);
          inline static auto gravity = glm::vec2(0.0f, -9.81f);

          Physics() = default;
      };

      /// Static class containing all the settings related to the input system
      class Input final {
        friend class Settings;
        public:
          /// Changes if the game allows mouse inputs or not
          static void SetAllowMouseInput(bool newState);
          /// Changes if the game allows mouse keyboard or not
          static void SetAllowKeyboardInput(bool newState);
          /// Changes if the game allows mouse gamepad or not
          static void SetAllowGamepadInput(bool newState);
          /// Changes the minimal value the sticks x and y must take to be considered as moved, clamped between -1.0f and 1.0f.
          static void SetGamepadStickThreshold(float newValue);
          /// Changes the minimal value the left and right triggers must take to be considered pressed, clamped between -1.0f and 1.0f.
          static void SetGamepadTriggerThreshold(float newValue);

          /// @returns True if the game allows mouse input, False if not
          [[nodiscard]] static bool AllowMouseInput();
          /// @returns True if the game allows keyboard input, False if not
          [[nodiscard]] static bool AllowKeyboardInput();
          /// @returns True if the game allows gamepad input, False if not
          [[nodiscard]] static bool AllowGamepadInput();
          /// @returns The value of the stick threshold of the gamepad
          [[nodiscard]] static float GamepadStickThreshold();
          /// @returns The value of the trigger threshold of the gamepad
          [[nodiscard]] static float GamepadTriggerThreshold();
        private:
          inline static bool allowMouseInput = true;
          inline static bool allowKeyboardInput = true;
          inline static bool allowGamepadInput = true;
          inline static float gamepadStickThreshold = 0.15f;
          inline static float gamepadTriggerThreshold = -1.0f;

          Input() = default;
      };

      class Profiling final {
        public:
          enum class Level {
            Disabled = 0, PerThread, PerSystem, PerSubSystem, PerFunction
          };

          /// Set which level of profiling you want to use:
          /// - Disabled: no profiling
          /// - PerThread: only the main function of each thread
          /// - PerSystem: only the main functions of each thread and system
          /// - PerSubSystem: only the main functions of each thread, system and subsystem
          /// - PerFunction: most functions of each thread, system and subsystem
          static void SetProfilingLevel(Level newValue);
          /// @returns The current profiling level the profiler uses
          [[nodiscard]] static Level ProfilingLevel();
          /// Defines the minimum execution time threshold (in microseconds) for a function to be recorded by the profiler.
          static void SetProfilingThreshold(unsigned long long newValue);
          /// @returns The minimum execution time threshold (in microseconds) used by the profiler
          [[nodiscard]] static unsigned long long ProfilingThreshold();
        private:
          inline static auto profilingLevel = Level::Disabled;
          inline static unsigned long long profilingThreshold = 50;
      };
    private:
      Settings() = default;
  };
}

using ProfilingLevel = Engine::Settings::Profiling::Level;

#endif //SETTINGS_H
