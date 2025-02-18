//
// Gamepad.cpp
// Author: Antoine Bastide
// Date: 19/11/2024
//

#include "Engine/Input/Gamepad.h"
#include "Engine/Settings.h"

namespace Engine::Input {
  GamepadButtonEvent Gamepad::BUTTON_NORTH{};
  GamepadButtonEvent Gamepad::BUTTON_SOUTH{};
  GamepadButtonEvent Gamepad::BUTTON_LEFT{};
  GamepadButtonEvent Gamepad::BUTTON_RIGHT{};
  GamepadButtonEvent Gamepad::PAD_LEFT{};
  GamepadButtonEvent Gamepad::PAD_RIGHT{};
  GamepadButtonEvent Gamepad::PAD_NORTH{};
  GamepadButtonEvent Gamepad::PAD_SOUTH{};
  GamepadButtonEvent Gamepad::LEFT_TRIGGER{};
  GamepadButtonEvent Gamepad::RIGHT_TRIGGER{};
  GamepadButtonEvent Gamepad::LEFT_BUMPER{};
  GamepadButtonEvent Gamepad::RIGHT_BUMPER{};
  GamepadButtonEvent Gamepad::START_BUTTON{};
  GamepadButtonEvent Gamepad::SELECT_BUTTON{};
  GamepadButtonEvent Gamepad::LEFT_STICK_BUTTON{};
  GamepadButtonEvent Gamepad::RIGHT_STICK_BUTTON{};
  GamepadButtonEvent Gamepad::CENTER_BUTTON{};
  GamepadStickEvent Gamepad::LEFT_STICK{};
  GamepadStickEvent Gamepad::RIGHT_STICK{};

  std::bitset<GLFW_GAMEPAD_BUTTON_DPAD_LEFT + 3> Gamepad::previousKeyStates{};
  GLFWgamepadstate Gamepad::state{};

  void Gamepad::processInput() {
    if (glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
      if (glfwGetGamepadState(GLFW_JOYSTICK_1, &state)) {
        processButton(GLFW_GAMEPAD_BUTTON_A, &BUTTON_SOUTH);
        processButton(GLFW_GAMEPAD_BUTTON_B, &BUTTON_RIGHT);
        processButton(GLFW_GAMEPAD_BUTTON_X, &BUTTON_LEFT);
        processButton(GLFW_GAMEPAD_BUTTON_Y, &BUTTON_NORTH);

        processButton(GLFW_GAMEPAD_BUTTON_LEFT_BUMPER, &LEFT_BUMPER);
        processButton(GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER, &RIGHT_BUMPER);

        processButton(GLFW_GAMEPAD_BUTTON_BACK, &SELECT_BUTTON);
        processButton(GLFW_GAMEPAD_BUTTON_START, &START_BUTTON);
        processButton(GLFW_GAMEPAD_BUTTON_GUIDE, &CENTER_BUTTON);

        processButton(GLFW_GAMEPAD_BUTTON_LEFT_THUMB, &LEFT_STICK_BUTTON);
        processButton(GLFW_GAMEPAD_BUTTON_RIGHT_THUMB, &RIGHT_STICK_BUTTON);

        processButton(GLFW_GAMEPAD_BUTTON_DPAD_UP, &PAD_NORTH);
        processButton(GLFW_GAMEPAD_BUTTON_DPAD_RIGHT, &PAD_RIGHT);
        processButton(GLFW_GAMEPAD_BUTTON_DPAD_DOWN, &PAD_SOUTH);
        processButton(GLFW_GAMEPAD_BUTTON_DPAD_LEFT, &PAD_LEFT);

        processStick(GLFW_GAMEPAD_AXIS_LEFT_X, GLFW_GAMEPAD_AXIS_LEFT_Y, &LEFT_STICK);
        processStick(GLFW_GAMEPAD_AXIS_RIGHT_X, GLFW_GAMEPAD_AXIS_RIGHT_Y, &RIGHT_STICK);

        processButton(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, &LEFT_TRIGGER, true);
        processButton(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, &RIGHT_TRIGGER, true);
      }
    }
  }

  void Gamepad::processButton(const int keyCode, GamepadButtonEvent *event, const bool useAxes) {
    // Get previous state
    const bool isPressed = useAxes
                             ? state.axes[keyCode] > Settings::Input::GetGamepadTriggerThreshold()
                             : state.buttons[keyCode] == GLFW_PRESS;
    const int updatedKeyCode = useAxes ? GLFW_GAMEPAD_BUTTON_LAST + keyCode - GLFW_GAMEPAD_AXIS_LEFT_TRIGGER + 1 : keyCode;
    const bool wasPressed = previousKeyStates[updatedKeyCode];
    previousKeyStates[updatedKeyCode] = isPressed;

    const KeyAndButtonContext ctx = {
      .pressed = !wasPressed && isPressed,
      .held = wasPressed && isPressed,
      .released = wasPressed && !isPressed
    };
    event->trigger(ctx);
  }

  void Gamepad::processStick(const int keyCodeX, const int keyCodeY, GamepadStickEvent *event) {
    const float x = axisTriggered(state.axes[keyCodeX]) ? state.axes[keyCodeX] : 0.0f;
    const float y = axisTriggered(state.axes[keyCodeY]) ? state.axes[keyCodeY] : 0.0f;
    event->trigger(glm::vec2(x, y));
  }

  bool Gamepad::axisTriggered(const float value) {
    const float axisThreshold = Settings::Input::GetGamepadStickThreshold();
    return value < -axisThreshold || axisThreshold < value;
  }
}
