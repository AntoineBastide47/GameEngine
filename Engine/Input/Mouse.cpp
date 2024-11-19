//
// Mouse.cpp
// Author: Antoine Bastide
// Date: 18/11/2024
//

#include "Input/Mouse.h"

namespace Engine::Input {
  MouseButtonEvent Mouse::LEFT{};
  MouseButtonEvent Mouse::RIGHT{};
  MouseButtonEvent Mouse::MIDDLE{};
  MouseButtonEvent Mouse::BUTTON_4{};
  MouseButtonEvent Mouse::BUTTON_5{};
  MouseButtonEvent Mouse::BUTTON_6{};
  MouseButtonEvent Mouse::BUTTON_7{};
  MouseButtonEvent Mouse::BUTTON_8{};
  MousePositionEvent Mouse::POSITION{};
  MouseScrollEvent Mouse::SCROLL{};
  KeyboardAndMouseContext Mouse::ctx{};

  double Mouse::x{};
  double Mouse::y{};
  GLFWwindow *Mouse::window = nullptr;
  std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> Mouse::previousKeyStates{};

  void Mouse::SetMouseNormal() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }

  void Mouse::SetMouseHidden() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  }

  void Mouse::SetMouseDisabled() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }

  void Mouse::initialize(GLFWwindow *window) {
    Mouse::window = window;
  }

  void Mouse::processButton(const int keyCode, MouseButtonEvent *event) {
    const int state = glfwGetMouseButton(window, keyCode);
    const bool isPressed = state == GLFW_PRESS;

    // Get previous state
    const bool wasPressed = previousKeyStates[keyCode];
    previousKeyStates[keyCode] = isPressed;

    // Process action if it's valid
    if (wasPressed || isPressed) {
      ctx.pressed = !wasPressed && isPressed;
      ctx.held = wasPressed && isPressed;
      ctx.released = wasPressed && !isPressed;
      event->Trigger(ctx);
    }
  }

  void Mouse::processPosition() {
    glfwGetCursorPos(window, &x, &y);
    POSITION.Trigger(Engine2D::Vector2{static_cast<float>(x), static_cast<float>(y)});
  }

  void Mouse::processInput() {
    ctx = {
      .leftShiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_RELEASE,
      .rightShiftPressed = glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) != GLFW_RELEASE,
      .leftControlPressed = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_RELEASE,
      .rightControlPressed = glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) != GLFW_RELEASE,
      .leftAltPressed = glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_RELEASE,
      .rightAltPressed = glfwGetKey(window, GLFW_KEY_RIGHT_ALT) != GLFW_RELEASE,
      .leftSuperPressed = glfwGetKey(window, GLFW_KEY_LEFT_SUPER) != GLFW_RELEASE,
      .rightSuperPressed = glfwGetKey(window, GLFW_KEY_RIGHT_SUPER) != GLFW_RELEASE,
      .capsLockPressed = glfwGetKey(window, GLFW_KEY_CAPS_LOCK) != GLFW_RELEASE,
      .numLockPressed = glfwGetKey(window, GLFW_KEY_NUM_LOCK) != GLFW_RELEASE,
    };

    processButton(GLFW_MOUSE_BUTTON_1, &LEFT);
    processButton(GLFW_MOUSE_BUTTON_2, &RIGHT);
    processButton(GLFW_MOUSE_BUTTON_3, &MIDDLE);
    processButton(GLFW_MOUSE_BUTTON_4, &BUTTON_4);
    processButton(GLFW_MOUSE_BUTTON_5, &BUTTON_5);
    processButton(GLFW_MOUSE_BUTTON_6, &BUTTON_6);
    processButton(GLFW_MOUSE_BUTTON_7, &BUTTON_7);
    processButton(GLFW_MOUSE_BUTTON_8, &BUTTON_8);
    processPosition();
  }

  void Mouse::processScroll(const double scroll) {
    SCROLL.Trigger(MouseScroll{scroll > 0, scroll < 0});
  }
}
