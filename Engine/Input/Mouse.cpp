//
// Mouse.cpp
// Author: Antoine Bastide
// Date: 18/11/2024
//

#include "Input/Mouse.h"

namespace Engine::Input {
  MouseContextEvent Mouse::LEFT{};
  MouseContextEvent Mouse::RIGHT{};
  MouseContextEvent Mouse::MIDDLE{};
  MouseContextEvent Mouse::BUTTON_4{};
  MouseContextEvent Mouse::BUTTON_5{};
  MouseContextEvent Mouse::BUTTON_6{};
  MouseContextEvent Mouse::BUTTON_7{};
  MouseContextEvent Mouse::BUTTON_8{};
  MousePositionEvent Mouse::POSITION{};
  MouseScrollEvent Mouse::SCROLL{};

  double Mouse::x{};
  double Mouse::y{};
  GLFWwindow *Mouse::window = nullptr;
  bool Mouse::previousKeyStates[GLFW_MOUSE_BUTTON_8 + 1] = {false};

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

  void Mouse::processButton(const int keyCode, MouseContextEvent *event) {
    const int state = glfwGetMouseButton(window, keyCode);
    const bool isPressed = state == GLFW_PRESS;

    // Get previous state
    const bool wasPressed = previousKeyStates[keyCode];
    previousKeyStates[keyCode] = isPressed;

    // Process action if it's valid
    if (wasPressed || isPressed) {
      const MouseContext context = {
        .pressed = !wasPressed && isPressed,
        .held = wasPressed && isPressed,
        .released = wasPressed && !isPressed,
      };
      event->Trigger(context);
    }
  }

  void Mouse::processPosition() {
    glfwGetCursorPos(window, &x, &y);
    POSITION.Trigger(Engine2D::Vector2{static_cast<float>(x), static_cast<float>(y)});
  }

  void Mouse::processInput() {
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
