//
// Gamepad.h
// Author: Antoine Bastide
// Date: 19/11/2024
//

#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <bitset>

#include "Engine2D/Types/Vector2.h"
#include "Common/Event.h"
#include "Common/RenderingHeaders.h"
#include "Input/InputContexts.h"

namespace Engine2D {
  class Game2D;
}

namespace Engine::Input {
  class GamepadButtonEvent final : public Event<KeyAndButtonContext> {
    friend class Gamepad;
  };
  class GamepadStickEvent final : public Event<Engine2D::Vector2f> {
    friend class Gamepad;
  };

  /**
   * The Gamepad class manages gamepad events, such as button presses and releases,
   * and provides a system for registering callbacks to respond to these events.
   * The gamepad left stick, right stick, and button all have an associated Event that triggers when they are updated.
   */
  class Gamepad final {
    friend class Engine2D::Game2D;
    public:
      static GamepadButtonEvent BUTTON_NORTH, BUTTON_SOUTH, BUTTON_LEFT, BUTTON_RIGHT;
      static GamepadButtonEvent PAD_LEFT, PAD_RIGHT, PAD_NORTH, PAD_SOUTH;
      static GamepadButtonEvent LEFT_TRIGGER, RIGHT_TRIGGER;
      static GamepadButtonEvent LEFT_BUMPER, RIGHT_BUMPER;
      static GamepadButtonEvent START_BUTTON, SELECT_BUTTON;
      static GamepadButtonEvent LEFT_STICK_BUTTON, RIGHT_STICK_BUTTON, CENTER_BUTTON;
      static GamepadStickEvent LEFT_STICK, RIGHT_STICK;
    private:
      /// If any of the gamepad buttons were pressed last frame
      static std::bitset<GLFW_GAMEPAD_BUTTON_DPAD_LEFT + 2 + 1> previousKeyStates;
      /// The current state of the gamepad
      static GLFWgamepadstate state;

      Gamepad() = default;

      /// Processes all the inputs of the current frame of the game and calls the corresponding gamepad events.
      static void processInput();
      /// Processes the given gamepad button input
      static void processButton(int keyCode, GamepadButtonEvent *event, bool useAxes = false);
      /// Processes the given gamepad stick input
      static void processStick(int keyCodeX, int keyCodeY, GamepadStickEvent *event);
      /// True if the axe was greater than the axisThreshold value, False if not
      static bool axisTriggered(float value);
  };
}

#endif //GAMEPAD_H
