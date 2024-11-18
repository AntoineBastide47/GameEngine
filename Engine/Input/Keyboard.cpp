//
// Keyboard.cpp
// Author: Antoine Bastide
// Date: 16/11/2024
//

#include "Input/Keyboard.h"
#include <ranges>

namespace Engine::Input {
  KeyboardEvent Keyboard::A{};
  KeyboardEvent Keyboard::B{};
  KeyboardEvent Keyboard::C{};
  KeyboardEvent Keyboard::D{};
  KeyboardEvent Keyboard::E{};
  KeyboardEvent Keyboard::F{};
  KeyboardEvent Keyboard::G{};
  KeyboardEvent Keyboard::H{};
  KeyboardEvent Keyboard::I{};
  KeyboardEvent Keyboard::J{};
  KeyboardEvent Keyboard::K{};
  KeyboardEvent Keyboard::L{};
  KeyboardEvent Keyboard::M{};
  KeyboardEvent Keyboard::N{};
  KeyboardEvent Keyboard::O{};
  KeyboardEvent Keyboard::P{};
  KeyboardEvent Keyboard::Q{};
  KeyboardEvent Keyboard::R{};
  KeyboardEvent Keyboard::S{};
  KeyboardEvent Keyboard::T{};
  KeyboardEvent Keyboard::U{};
  KeyboardEvent Keyboard::V{};
  KeyboardEvent Keyboard::W{};
  KeyboardEvent Keyboard::X{};
  KeyboardEvent Keyboard::Y{};
  KeyboardEvent Keyboard::Z{};
  KeyboardEvent Keyboard::NUM0{};
  KeyboardEvent Keyboard::NUM1{};
  KeyboardEvent Keyboard::NUM2{};
  KeyboardEvent Keyboard::NUM3{};
  KeyboardEvent Keyboard::NUM4{};
  KeyboardEvent Keyboard::NUM5{};
  KeyboardEvent Keyboard::NUM6{};
  KeyboardEvent Keyboard::NUM7{};
  KeyboardEvent Keyboard::NUM8{};
  KeyboardEvent Keyboard::NUM9{};
  KeyboardEvent Keyboard::SPACE{};
  KeyboardEvent Keyboard::APOSTROPHE{};
  KeyboardEvent Keyboard::COMMA{};
  KeyboardEvent Keyboard::MINUS{};
  KeyboardEvent Keyboard::PERIOD{};
  KeyboardEvent Keyboard::SLASH{};
  KeyboardEvent Keyboard::SEMICOLON{};
  KeyboardEvent Keyboard::EQUAL{};
  KeyboardEvent Keyboard::LEFT_BRACKET{};
  KeyboardEvent Keyboard::BACKSLASH{};
  KeyboardEvent Keyboard::RIGHT_BRACKET{};
  KeyboardEvent Keyboard::GRAVE_ACCENT{};
  KeyboardEvent Keyboard::ESCAPE{};
  KeyboardEvent Keyboard::ENTER{};
  KeyboardEvent Keyboard::TAB{};
  KeyboardEvent Keyboard::BACKSPACE{};
  KeyboardEvent Keyboard::INSERT{};
  KeyboardEvent Keyboard::DELETE{};
  KeyboardEvent Keyboard::RIGHT{};
  KeyboardEvent Keyboard::LEFT{};
  KeyboardEvent Keyboard::DOWN{};
  KeyboardEvent Keyboard::UP{};
  KeyboardEvent Keyboard::PAGE_UP{};
  KeyboardEvent Keyboard::PAGE_DOWN{};
  KeyboardEvent Keyboard::HOME{};
  KeyboardEvent Keyboard::END{};
  KeyboardEvent Keyboard::CAPS_LOCK{};
  KeyboardEvent Keyboard::SCROLL_LOCK{};
  KeyboardEvent Keyboard::NUM_LOCK{};
  KeyboardEvent Keyboard::PRINT_SCREEN{};
  KeyboardEvent Keyboard::PAUSE{};
  KeyboardEvent Keyboard::F1{};
  KeyboardEvent Keyboard::F2{};
  KeyboardEvent Keyboard::F3{};
  KeyboardEvent Keyboard::F4{};
  KeyboardEvent Keyboard::F5{};
  KeyboardEvent Keyboard::F6{};
  KeyboardEvent Keyboard::F7{};
  KeyboardEvent Keyboard::F8{};
  KeyboardEvent Keyboard::F9{};
  KeyboardEvent Keyboard::F10{};
  KeyboardEvent Keyboard::F11{};
  KeyboardEvent Keyboard::F12{};
  KeyboardEvent Keyboard::F13{};
  KeyboardEvent Keyboard::F14{};
  KeyboardEvent Keyboard::F15{};
  KeyboardEvent Keyboard::F16{};
  KeyboardEvent Keyboard::F17{};
  KeyboardEvent Keyboard::F18{};
  KeyboardEvent Keyboard::F19{};
  KeyboardEvent Keyboard::F20{};
  KeyboardEvent Keyboard::F21{};
  KeyboardEvent Keyboard::F22{};
  KeyboardEvent Keyboard::F23{};
  KeyboardEvent Keyboard::F24{};
  KeyboardEvent Keyboard::F25{};
  KeyboardEvent Keyboard::KP_NUM0{};
  KeyboardEvent Keyboard::KP_NUM1{};
  KeyboardEvent Keyboard::KP_NUM2{};
  KeyboardEvent Keyboard::KP_NUM3{};
  KeyboardEvent Keyboard::KP_NUM4{};
  KeyboardEvent Keyboard::KP_NUM5{};
  KeyboardEvent Keyboard::KP_NUM6{};
  KeyboardEvent Keyboard::KP_NUM7{};
  KeyboardEvent Keyboard::KP_NUM8{};
  KeyboardEvent Keyboard::KP_NUM9{};
  KeyboardEvent Keyboard::KP_DECIMAL{};
  KeyboardEvent Keyboard::KP_DIVIDE{};
  KeyboardEvent Keyboard::KP_MULTIPLY{};
  KeyboardEvent Keyboard::KP_SUBTRACT{};
  KeyboardEvent Keyboard::KP_ADD{};
  KeyboardEvent Keyboard::KP_ENTER{};
  KeyboardEvent Keyboard::KP_EQUAL{};
  KeyboardEvent Keyboard::LEFT_SHIFT{};
  KeyboardEvent Keyboard::LEFT_CONTROL{};
  KeyboardEvent Keyboard::LEFT_ALT{};
  KeyboardEvent Keyboard::LEFT_SUPER{};
  KeyboardEvent Keyboard::RIGHT_SHIFT{};
  KeyboardEvent Keyboard::RIGHT_CONTROL{};
  KeyboardEvent Keyboard::RIGHT_ALT{};
  KeyboardEvent Keyboard::RIGHT_SUPER{};
  KeyboardEvent Keyboard::MENU{};

  GLFWwindow *Keyboard::window = nullptr;
  bool Keyboard::previousKeyStates[GLFW_KEY_LAST + 1] = {false};
  std::unordered_map<char, KeyboardEvent *> Keyboard::variableKeyEvents;

  void Keyboard::initialize(GLFWwindow *window) {
    Keyboard::window = window;

    variableKeyEvents.insert(std::pair('a', &A));
    variableKeyEvents.insert(std::pair('b', &B));
    variableKeyEvents.insert(std::pair('c', &C));
    variableKeyEvents.insert(std::pair('d', &D));
    variableKeyEvents.insert(std::pair('e', &E));
    variableKeyEvents.insert(std::pair('f', &F));
    variableKeyEvents.insert(std::pair('g', &G));
    variableKeyEvents.insert(std::pair('h', &H));
    variableKeyEvents.insert(std::pair('i', &I));
    variableKeyEvents.insert(std::pair('j', &J));
    variableKeyEvents.insert(std::pair('k', &K));
    variableKeyEvents.insert(std::pair('l', &L));
    variableKeyEvents.insert(std::pair('m', &M));
    variableKeyEvents.insert(std::pair('n', &N));
    variableKeyEvents.insert(std::pair('o', &O));
    variableKeyEvents.insert(std::pair('p', &P));
    variableKeyEvents.insert(std::pair('q', &Q));
    variableKeyEvents.insert(std::pair('r', &R));
    variableKeyEvents.insert(std::pair('s', &S));
    variableKeyEvents.insert(std::pair('t', &T));
    variableKeyEvents.insert(std::pair('u', &U));
    variableKeyEvents.insert(std::pair('v', &V));
    variableKeyEvents.insert(std::pair('w', &W));
    variableKeyEvents.insert(std::pair('x', &X));
    variableKeyEvents.insert(std::pair('y', &Y));
    variableKeyEvents.insert(std::pair('z', &Z));
    variableKeyEvents.insert(std::pair('\'', &APOSTROPHE));
    variableKeyEvents.insert(std::pair(',', &COMMA));
    variableKeyEvents.insert(std::pair('-', &MINUS));
    variableKeyEvents.insert(std::pair('.', &PERIOD));
    variableKeyEvents.insert(std::pair('/', &SLASH));
    variableKeyEvents.insert(std::pair(';', &SEMICOLON));
    variableKeyEvents.insert(std::pair('=', &EQUAL));
    variableKeyEvents.insert(std::pair('[', &LEFT_BRACKET));
    variableKeyEvents.insert(std::pair('\\', &BACKSLASH));
    variableKeyEvents.insert(std::pair(']', &RIGHT_BRACKET));
    variableKeyEvents.insert(std::pair('`', &GRAVE_ACCENT));
  }

  void Keyboard::processInput() {
    // Update modifier keys
    const bool leftShiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_RELEASE;
    const bool rightShiftPressed = glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) != GLFW_RELEASE;
    const bool leftControlPressed = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_RELEASE;
    const bool rightControlPressed = glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) != GLFW_RELEASE;
    const bool leftAltPressed = glfwGetKey(window, GLFW_KEY_LEFT_ALT) != GLFW_RELEASE;
    const bool rightAltPressed = glfwGetKey(window, GLFW_KEY_RIGHT_ALT) != GLFW_RELEASE;
    const bool leftSuperPressed = glfwGetKey(window, GLFW_KEY_LEFT_SUPER) != GLFW_RELEASE;
    const bool rightSuperPressed = glfwGetKey(window, GLFW_KEY_RIGHT_SUPER) != GLFW_RELEASE;
    const bool capsLockPressed = glfwGetKey(window, GLFW_KEY_CAPS_LOCK) != GLFW_RELEASE;
    const bool numLockPressed = glfwGetKey(window, GLFW_KEY_NUM_LOCK) != GLFW_RELEASE;

    // For each key of interest
    auto processKey = [&](const int keyCode, KeyboardEvent *event) {
      const int state = glfwGetKey(window, keyCode);
      const char *keyName = glfwGetKeyName(keyCode, glfwGetKeyScancode(keyCode));
      const bool isPressed = state == GLFW_PRESS;

      // Get previous state
      const bool wasPressed = previousKeyStates[keyCode];
      previousKeyStates[keyCode] = isPressed;

      // Process action if it's valid
      if (wasPressed || isPressed) {
        const KeyboardContext context = {
          .pressed = !wasPressed && isPressed,
          .held = wasPressed && isPressed,
          .released = wasPressed && !isPressed,
          .leftShiftPressed = leftShiftPressed,
          .rightShiftPressed = rightShiftPressed,
          .leftControlPressed = leftControlPressed,
          .rightControlPressed = rightControlPressed,
          .leftAltPressed = leftAltPressed,
          .rightAltPressed = rightAltPressed,
          .leftSuperPressed = leftSuperPressed,
          .rightSuperPressed = rightSuperPressed,
          .capsLockPressed = capsLockPressed,
          .numLockPressed = numLockPressed,
        };
        (keyName && variableKeyEvents.contains(*keyName) ? variableKeyEvents[*keyName] : event)->Trigger(context);
      }
    };

    for (int keyCode = GLFW_KEY_SPACE; keyCode <= GLFW_KEY_LAST; ++keyCode) {
      switch (keyCode) {
        case GLFW_KEY_A: processKey(keyCode, &A);
          break;
        case GLFW_KEY_B: processKey(keyCode, &B);
          break;
        case GLFW_KEY_C: processKey(keyCode, &C);
          break;
        case GLFW_KEY_D: processKey(keyCode, &D);
          break;
        case GLFW_KEY_E: processKey(keyCode, &E);
          break;
        case GLFW_KEY_F: processKey(keyCode, &F);
          break;
        case GLFW_KEY_G: processKey(keyCode, &G);
          break;
        case GLFW_KEY_H: processKey(keyCode, &H);
          break;
        case GLFW_KEY_I: processKey(keyCode, &I);
          break;
        case GLFW_KEY_J: processKey(keyCode, &J);
          break;
        case GLFW_KEY_K: processKey(keyCode, &K);
          break;
        case GLFW_KEY_L: processKey(keyCode, &L);
          break;
        case GLFW_KEY_M: processKey(keyCode, &M);
          break;
        case GLFW_KEY_N: processKey(keyCode, &N);
          break;
        case GLFW_KEY_O: processKey(keyCode, &O);
          break;
        case GLFW_KEY_P: processKey(keyCode, &P);
          break;
        case GLFW_KEY_Q: processKey(keyCode, &Q);
          break;
        case GLFW_KEY_R: processKey(keyCode, &R);
          break;
        case GLFW_KEY_S: processKey(keyCode, &S);
          break;
        case GLFW_KEY_T: processKey(keyCode, &T);
          break;
        case GLFW_KEY_U: processKey(keyCode, &U);
          break;
        case GLFW_KEY_V: processKey(keyCode, &V);
          break;
        case GLFW_KEY_W: processKey(keyCode, &W);
          break;
        case GLFW_KEY_X: processKey(keyCode, &X);
          break;
        case GLFW_KEY_Y: processKey(keyCode, &Y);
          break;
        case GLFW_KEY_Z: processKey(keyCode, &Z);
          break;
        case GLFW_KEY_0: processKey(keyCode, &NUM0);
          break;
        case GLFW_KEY_1: processKey(keyCode, &NUM1);
          break;
        case GLFW_KEY_2: processKey(keyCode, &NUM2);
          break;
        case GLFW_KEY_3: processKey(keyCode, &NUM3);
          break;
        case GLFW_KEY_4: processKey(keyCode, &NUM4);
          break;
        case GLFW_KEY_5: processKey(keyCode, &NUM5);
          break;
        case GLFW_KEY_6: processKey(keyCode, &NUM6);
          break;
        case GLFW_KEY_7: processKey(keyCode, &NUM7);
          break;
        case GLFW_KEY_8: processKey(keyCode, &NUM8);
          break;
        case GLFW_KEY_9: processKey(keyCode, &NUM9);
          break;
        case GLFW_KEY_SPACE: processKey(keyCode, &SPACE);
          break;
        case GLFW_KEY_APOSTROPHE: processKey(keyCode, &APOSTROPHE);
          break;
        case GLFW_KEY_COMMA: processKey(keyCode, &COMMA);
          break;
        case GLFW_KEY_MINUS: processKey(keyCode, &MINUS);
          break;
        case GLFW_KEY_PERIOD: processKey(keyCode, &PERIOD);
          break;
        case GLFW_KEY_SLASH: processKey(keyCode, &SLASH);
          break;
        case GLFW_KEY_SEMICOLON: processKey(keyCode, &SEMICOLON);
          break;
        case GLFW_KEY_EQUAL: processKey(keyCode, &EQUAL);
          break;
        case GLFW_KEY_LEFT_BRACKET: processKey(keyCode, &LEFT_BRACKET);
          break;
        case GLFW_KEY_BACKSLASH: processKey(keyCode, &BACKSLASH);
          break;
        case GLFW_KEY_RIGHT_BRACKET: processKey(keyCode, &RIGHT_BRACKET);
          break;
        case GLFW_KEY_GRAVE_ACCENT: processKey(keyCode, &GRAVE_ACCENT);
          break;
        case GLFW_KEY_ESCAPE: processKey(keyCode, &ESCAPE);
          break;
        case GLFW_KEY_ENTER: processKey(keyCode, &ENTER);
          break;
        case GLFW_KEY_TAB: processKey(keyCode, &TAB);
          break;
        case GLFW_KEY_BACKSPACE: processKey(keyCode, &BACKSPACE);
          break;
        case GLFW_KEY_INSERT: processKey(keyCode, &INSERT);
          break;
        case GLFW_KEY_DELETE: processKey(keyCode, &DELETE);
          break;
        case GLFW_KEY_RIGHT: processKey(keyCode, &RIGHT);
          break;
        case GLFW_KEY_LEFT: processKey(keyCode, &LEFT);
          break;
        case GLFW_KEY_DOWN: processKey(keyCode, &DOWN);
          break;
        case GLFW_KEY_UP: processKey(keyCode, &UP);
          break;
        case GLFW_KEY_PAGE_UP: processKey(keyCode, &PAGE_UP);
          break;
        case GLFW_KEY_PAGE_DOWN: processKey(keyCode, &PAGE_DOWN);
          break;
        case GLFW_KEY_HOME: processKey(keyCode, &HOME);
          break;
        case GLFW_KEY_END: processKey(keyCode, &END);
          break;
        case GLFW_KEY_CAPS_LOCK: processKey(keyCode, &CAPS_LOCK);
          break;
        case GLFW_KEY_SCROLL_LOCK: processKey(keyCode, &SCROLL_LOCK);
          break;
        case GLFW_KEY_NUM_LOCK: processKey(keyCode, &NUM_LOCK);
          break;
        case GLFW_KEY_PRINT_SCREEN: processKey(keyCode, &PRINT_SCREEN);
          break;
        case GLFW_KEY_PAUSE: processKey(keyCode, &PAUSE);
          break;
        case GLFW_KEY_F1: processKey(keyCode, &F1);
          break;
        case GLFW_KEY_F2: processKey(keyCode, &F2);
          break;
        case GLFW_KEY_F3: processKey(keyCode, &F3);
          break;
        case GLFW_KEY_F4: processKey(keyCode, &F4);
          break;
        case GLFW_KEY_F5: processKey(keyCode, &F5);
          break;
        case GLFW_KEY_F6: processKey(keyCode, &F6);
          break;
        case GLFW_KEY_F7: processKey(keyCode, &F7);
          break;
        case GLFW_KEY_F8: processKey(keyCode, &F8);
          break;
        case GLFW_KEY_F9: processKey(keyCode, &F9);
          break;
        case GLFW_KEY_F10: processKey(keyCode, &F10);
          break;
        case GLFW_KEY_F11: processKey(keyCode, &F11);
          break;
        case GLFW_KEY_F12: processKey(keyCode, &F12);
          break;
        case GLFW_KEY_F13: processKey(keyCode, &F13);
          break;
        case GLFW_KEY_F14: processKey(keyCode, &F14);
          break;
        case GLFW_KEY_F15: processKey(keyCode, &F15);
          break;
        case GLFW_KEY_F16: processKey(keyCode, &F16);
          break;
        case GLFW_KEY_F17: processKey(keyCode, &F17);
          break;
        case GLFW_KEY_F18: processKey(keyCode, &F18);
          break;
        case GLFW_KEY_F19: processKey(keyCode, &F19);
          break;
        case GLFW_KEY_F20: processKey(keyCode, &F20);
          break;
        case GLFW_KEY_F21: processKey(keyCode, &F21);
          break;
        case GLFW_KEY_F22: processKey(keyCode, &F22);
          break;
        case GLFW_KEY_F23: processKey(keyCode, &F23);
          break;
        case GLFW_KEY_F24: processKey(keyCode, &F24);
          break;
        case GLFW_KEY_F25: processKey(keyCode, &F25);
          break;
        case GLFW_KEY_KP_0: processKey(keyCode, &KP_NUM0);
          break;
        case GLFW_KEY_KP_1: processKey(keyCode, &KP_NUM1);
          break;
        case GLFW_KEY_KP_2: processKey(keyCode, &KP_NUM2);
          break;
        case GLFW_KEY_KP_3: processKey(keyCode, &KP_NUM3);
          break;
        case GLFW_KEY_KP_4: processKey(keyCode, &KP_NUM4);
          break;
        case GLFW_KEY_KP_5: processKey(keyCode, &KP_NUM5);
          break;
        case GLFW_KEY_KP_6: processKey(keyCode, &KP_NUM6);
          break;
        case GLFW_KEY_KP_7: processKey(keyCode, &KP_NUM7);
          break;
        case GLFW_KEY_KP_8: processKey(keyCode, &KP_NUM8);
          break;
        case GLFW_KEY_KP_9: processKey(keyCode, &KP_NUM9);
          break;
        case GLFW_KEY_KP_DECIMAL: processKey(keyCode, &KP_DECIMAL);
          break;
        case GLFW_KEY_KP_DIVIDE: processKey(keyCode, &KP_DIVIDE);
          break;
        case GLFW_KEY_KP_MULTIPLY: processKey(keyCode, &KP_MULTIPLY);
          break;
        case GLFW_KEY_KP_SUBTRACT: processKey(keyCode, &KP_SUBTRACT);
          break;
        case GLFW_KEY_KP_ADD: processKey(keyCode, &KP_ADD);
          break;
        case GLFW_KEY_KP_ENTER: processKey(keyCode, &KP_ENTER);
          break;
        case GLFW_KEY_KP_EQUAL: processKey(keyCode, &KP_EQUAL);
          break;
        case GLFW_KEY_LEFT_SHIFT: processKey(keyCode, &LEFT_SHIFT);
          break;
        case GLFW_KEY_LEFT_CONTROL: processKey(keyCode, &LEFT_CONTROL);
          break;
        case GLFW_KEY_LEFT_ALT: processKey(keyCode, &LEFT_ALT);
          break;
        case GLFW_KEY_LEFT_SUPER: processKey(keyCode, &LEFT_SUPER);
          break;
        case GLFW_KEY_RIGHT_SHIFT: processKey(keyCode, &RIGHT_SHIFT);
          break;
        case GLFW_KEY_RIGHT_CONTROL: processKey(keyCode, &RIGHT_CONTROL);
          break;
        case GLFW_KEY_RIGHT_ALT: processKey(keyCode, &RIGHT_ALT);
          break;
        case GLFW_KEY_RIGHT_SUPER: processKey(keyCode, &RIGHT_SUPER);
          break;
        case GLFW_KEY_MENU: processKey(keyCode, &MENU);
          break;
        default: break;
      }
    }
  }
}
