//
// Keyboard.cpp
// Author: Antoine Bastide
// Date: 16/11/2024
//

#include <ranges>

#include "Engine/Input/Keyboard.h"
#include "Engine/Macros/Utils.h"

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
  std::bitset<GLFW_KEY_LAST + 1> Keyboard::previousKeyStates{};
  std::unordered_map<char, KeyboardEvent *> Keyboard::variableKeyEvents;
  KeyboardAndMouseContext Keyboard::ctx{};

  Keyboard::~Keyboard() {
    window = nullptr;
    for (auto eventPtr: variableKeyEvents | std::views::values)
      SAFE_DELETE(eventPtr);
    variableKeyEvents.clear();
  }

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

  void Keyboard::processKey(const int keyCode, KeyboardEvent *event) {
    // Get previous state
    const char *keyName = glfwGetKeyName(keyCode, glfwGetKeyScancode(keyCode));
    const bool isPressed = glfwGetKey(window, keyCode) == GLFW_PRESS;
    const bool wasPressed = previousKeyStates[keyCode];
    previousKeyStates[keyCode] = isPressed;

    // Process action if it's valid
    if (wasPressed || isPressed) {
      ctx.pressed = !wasPressed && isPressed;
      ctx.held = wasPressed && isPressed;
      ctx.released = wasPressed && !isPressed;
      (keyName && variableKeyEvents.contains(*keyName) ? variableKeyEvents[*keyName] : event)->trigger(ctx);
    }
  }

  void Keyboard::processInput() {
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

    processKey(GLFW_KEY_A, &A);
    processKey(GLFW_KEY_B, &B);
    processKey(GLFW_KEY_C, &C);
    processKey(GLFW_KEY_D, &D);
    processKey(GLFW_KEY_E, &E);
    processKey(GLFW_KEY_F, &F);
    processKey(GLFW_KEY_G, &G);
    processKey(GLFW_KEY_H, &H);
    processKey(GLFW_KEY_I, &I);
    processKey(GLFW_KEY_J, &J);
    processKey(GLFW_KEY_K, &K);
    processKey(GLFW_KEY_L, &L);
    processKey(GLFW_KEY_M, &M);
    processKey(GLFW_KEY_N, &N);
    processKey(GLFW_KEY_O, &O);
    processKey(GLFW_KEY_P, &P);
    processKey(GLFW_KEY_Q, &Q);
    processKey(GLFW_KEY_R, &R);
    processKey(GLFW_KEY_S, &S);
    processKey(GLFW_KEY_T, &T);
    processKey(GLFW_KEY_U, &U);
    processKey(GLFW_KEY_V, &V);
    processKey(GLFW_KEY_W, &W);
    processKey(GLFW_KEY_X, &X);
    processKey(GLFW_KEY_Y, &Y);
    processKey(GLFW_KEY_Z, &Z);

    processKey(GLFW_KEY_0, &NUM0);
    processKey(GLFW_KEY_1, &NUM1);
    processKey(GLFW_KEY_2, &NUM2);
    processKey(GLFW_KEY_3, &NUM3);
    processKey(GLFW_KEY_4, &NUM4);
    processKey(GLFW_KEY_5, &NUM5);
    processKey(GLFW_KEY_6, &NUM6);
    processKey(GLFW_KEY_7, &NUM7);
    processKey(GLFW_KEY_8, &NUM8);
    processKey(GLFW_KEY_9, &NUM9);

    processKey(GLFW_KEY_SPACE, &SPACE);
    processKey(GLFW_KEY_APOSTROPHE, &APOSTROPHE);
    processKey(GLFW_KEY_COMMA, &COMMA);
    processKey(GLFW_KEY_MINUS, &MINUS);
    processKey(GLFW_KEY_PERIOD, &PERIOD);
    processKey(GLFW_KEY_SLASH, &SLASH);
    processKey(GLFW_KEY_SEMICOLON, &SEMICOLON);
    processKey(GLFW_KEY_EQUAL, &EQUAL);
    processKey(GLFW_KEY_LEFT_BRACKET, &LEFT_BRACKET);
    processKey(GLFW_KEY_BACKSLASH, &BACKSLASH);
    processKey(GLFW_KEY_RIGHT_BRACKET, &RIGHT_BRACKET);
    processKey(GLFW_KEY_GRAVE_ACCENT, &GRAVE_ACCENT);
    processKey(GLFW_KEY_ESCAPE, &ESCAPE);
    processKey(GLFW_KEY_ENTER, &ENTER);
    processKey(GLFW_KEY_TAB, &TAB);
    processKey(GLFW_KEY_BACKSPACE, &BACKSPACE);
    processKey(GLFW_KEY_INSERT, &INSERT);
    processKey(GLFW_KEY_DELETE, &DELETE);
    processKey(GLFW_KEY_RIGHT, &RIGHT);
    processKey(GLFW_KEY_LEFT, &LEFT);
    processKey(GLFW_KEY_DOWN, &DOWN);
    processKey(GLFW_KEY_UP, &UP);
    processKey(GLFW_KEY_PAGE_UP, &PAGE_UP);
    processKey(GLFW_KEY_PAGE_DOWN, &PAGE_DOWN);
    processKey(GLFW_KEY_HOME, &HOME);
    processKey(GLFW_KEY_END, &END);
    processKey(GLFW_KEY_CAPS_LOCK, &CAPS_LOCK);
    processKey(GLFW_KEY_SCROLL_LOCK, &SCROLL_LOCK);
    processKey(GLFW_KEY_NUM_LOCK, &NUM_LOCK);
    processKey(GLFW_KEY_PRINT_SCREEN, &PRINT_SCREEN);
    processKey(GLFW_KEY_PAUSE, &PAUSE);

    processKey(GLFW_KEY_F1, &F1);
    processKey(GLFW_KEY_F2, &F2);
    processKey(GLFW_KEY_F3, &F3);
    processKey(GLFW_KEY_F4, &F4);
    processKey(GLFW_KEY_F5, &F5);
    processKey(GLFW_KEY_F6, &F6);
    processKey(GLFW_KEY_F7, &F7);
    processKey(GLFW_KEY_F8, &F8);
    processKey(GLFW_KEY_F9, &F9);
    processKey(GLFW_KEY_F10, &F10);
    processKey(GLFW_KEY_F11, &F11);
    processKey(GLFW_KEY_F12, &F12);
    processKey(GLFW_KEY_F13, &F13);
    processKey(GLFW_KEY_F14, &F14);
    processKey(GLFW_KEY_F15, &F15);
    processKey(GLFW_KEY_F16, &F16);
    processKey(GLFW_KEY_F17, &F17);
    processKey(GLFW_KEY_F18, &F18);
    processKey(GLFW_KEY_F19, &F19);
    processKey(GLFW_KEY_F20, &F20);
    processKey(GLFW_KEY_F21, &F21);
    processKey(GLFW_KEY_F22, &F22);
    processKey(GLFW_KEY_F23, &F23);
    processKey(GLFW_KEY_F24, &F24);
    processKey(GLFW_KEY_F25, &F25);

    processKey(GLFW_KEY_KP_0, &KP_NUM0);
    processKey(GLFW_KEY_KP_1, &KP_NUM1);
    processKey(GLFW_KEY_KP_2, &KP_NUM2);
    processKey(GLFW_KEY_KP_3, &KP_NUM3);
    processKey(GLFW_KEY_KP_4, &KP_NUM4);
    processKey(GLFW_KEY_KP_5, &KP_NUM5);
    processKey(GLFW_KEY_KP_6, &KP_NUM6);
    processKey(GLFW_KEY_KP_7, &KP_NUM7);
    processKey(GLFW_KEY_KP_8, &KP_NUM8);
    processKey(GLFW_KEY_KP_9, &KP_NUM9);

    processKey(GLFW_KEY_KP_DECIMAL, &KP_DECIMAL);
    processKey(GLFW_KEY_KP_DIVIDE, &KP_DIVIDE);
    processKey(GLFW_KEY_KP_MULTIPLY, &KP_MULTIPLY);
    processKey(GLFW_KEY_KP_SUBTRACT, &KP_SUBTRACT);
    processKey(GLFW_KEY_KP_ADD, &KP_ADD);
    processKey(GLFW_KEY_KP_ENTER, &KP_ENTER);
    processKey(GLFW_KEY_KP_EQUAL, &KP_EQUAL);

    processKey(GLFW_KEY_LEFT_SHIFT, &LEFT_SHIFT);
    processKey(GLFW_KEY_LEFT_CONTROL, &LEFT_CONTROL);
    processKey(GLFW_KEY_LEFT_ALT, &LEFT_ALT);
    processKey(GLFW_KEY_LEFT_SUPER, &LEFT_SUPER);
    processKey(GLFW_KEY_RIGHT_SHIFT, &RIGHT_SHIFT);
    processKey(GLFW_KEY_RIGHT_CONTROL, &RIGHT_CONTROL);
    processKey(GLFW_KEY_RIGHT_ALT, &RIGHT_ALT);
    processKey(GLFW_KEY_RIGHT_SUPER, &RIGHT_SUPER);
    processKey(GLFW_KEY_MENU, &MENU);
  }
}
