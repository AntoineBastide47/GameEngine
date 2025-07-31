//
// Keyboard.h
// Author: Antoine Bastide
// Date: 16/11/2024
//

#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <bitset>

#include "Engine/Event.hpp"
#include "Engine/RenderingHeaders.hpp"
#include "Engine/Input/InputContexts.hpp"

namespace Engine2D {
  class Game2D;
  class Entity2D;
}

namespace Engine::Input {
  class KeyboardEvent final : public Event<KeyboardAndMouseContext> {
    friend class Keyboard;
  };

  /**
   * The Keyboard class manages keyboard events, such as key presses and releases,
   * and provides a system for registering callbacks to respond to these events.
   * The keys all have an associated Event that triggers when they are updated.
   */
  class Keyboard final {
    public:
      static KeyboardEvent A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, NUM0, NUM1,
          NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9, SPACE, APOSTROPHE, COMMA, MINUS, PERIOD, SLASH, SEMICOLON,
          EQUAL, LEFT_BRACKET, BACKSLASH, RIGHT_BRACKET, GRAVE_ACCENT, ESCAPE, ENTER, TAB, BACKSPACE, INSERT, DELETE,
          RIGHT, LEFT, DOWN, UP, PAGE_UP, PAGE_DOWN, HOME, END, CAPS_LOCK, SCROLL_LOCK, NUM_LOCK, PRINT_SCREEN, PAUSE,
          F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, F16, F17, F18, F19, F20, F21, F22, F23, F24,
          F25, KP_NUM0, KP_NUM1, KP_NUM2, KP_NUM3, KP_NUM4, KP_NUM5, KP_NUM6, KP_NUM7, KP_NUM8, KP_NUM9, KP_DECIMAL,
          KP_DIVIDE, KP_MULTIPLY, KP_SUBTRACT, KP_ADD, KP_ENTER, KP_EQUAL, LEFT_SHIFT, LEFT_CONTROL, LEFT_ALT,
          LEFT_SUPER, RIGHT_SHIFT, RIGHT_CONTROL, RIGHT_ALT, RIGHT_SUPER, MENU;

      friend class Engine2D::Game2D;
    private:
      /// If any of the keys were pressed last frame
      static std::bitset<GLFW_KEY_LAST + 1> previousKeyStates;
      /// A map that contains the keyboard events for the keys that can change based on the most common keyboard layouts
      static std::unordered_map<char, KeyboardEvent *> variableKeyEvents;
      /// The window pointer required for querying key states.
      static GLFWwindow *window;
      /// Reference to the current KeyboardAndMouseButtonContext to prevent heap allocation overhead
      static KeyboardAndMouseContext ctx;

      Keyboard() = default;
      ~Keyboard();

      /// Initializes the Mouse instance
      static void initialize(GLFWwindow *window);
      /// Processes all the inputs of the current frame of the game and calls the corresponding key events.
      static void processInput();
      /// Processes an individual key
      static void processKey(int keyCode, KeyboardEvent *event);
  };
}

#endif //KEYBOARD_H
