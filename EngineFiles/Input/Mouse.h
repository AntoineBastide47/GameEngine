//
// Mouse.h
// Author: Antoine Bastide
// Date: 18/11/2024
//

#ifndef MOUSE_H
#define MOUSE_H

#include "InputContexts.h"
#include "2D/Types/Vector2.h"
#include "Common/Event.h"
#include "Common/RenderingHeaders.h"

namespace Engine2D {
  class Game2D;
}

namespace Engine::Input {
  class MouseButtonEvent : public Event<KeyboardAndMouseContext> {
    friend class Mouse;
  };
  class MousePositionEvent : public Event<Engine2D::Vector2> {
    friend class Mouse;
  };
  class MouseScrollEvent : public Event<MouseScroll> {
    friend class Mouse;
  };

  /**
   * The Mouse class manages mouse events, such as mouse button presses and releases, mouse position and scroll,
   * and provides a system for registering callbacks to respond to these events.
   * The mouse position, scroll and button all have an associated Event that triggers when they are updated.
   */
  class Mouse {
    public:
      static MouseButtonEvent LEFT, MIDDLE, RIGHT, BUTTON_4, BUTTON_5, BUTTON_6, BUTTON_7, BUTTON_8;
      static MousePositionEvent POSITION;
      static MouseScrollEvent SCROLL;

      /** Sets the mouse lock state to false and hidden state to false. */
      static void SetMouseNormal();
      /** Sets the mouse lock state to false and hidden state to true. */
      static void SetMouseHidden();
      /** Sets the mouse lock state to true and hidden state to true. */
      static void SetMouseDisabled();

      friend class Engine2D::Game2D;
    private:
      /** If any of the mouse buttons were pressed last frame */
      static std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> previousKeyStates;
      /** The window pointer required for querying mouse states. */
      static GLFWwindow *window;
      /** Temporary reference to the x position of the mouse */
      static double x;
      /** Temporary reference to the y position of the mouse */
      static double y;
      /** Reference to the current KeyboardAndMouseButtonContext to prevent heap allocation overhead */
      static KeyboardAndMouseContext ctx;

      Mouse() = default;

      ~Mouse() {
        window = nullptr;
      }

      /** Initializes the Mouse instance */
      static void initialize(GLFWwindow *window);
      /** Processes all the inputs of the current frame of the game and calls the corresponding key events. */
      static void processInput();
      /** Processes an individual mouse button */
      static void processButton(int keyCode, MouseButtonEvent *event);
      /** Processes the movement of the mouse */
      static void processPosition();
      /** Processes the scrolling of the mouse */
      static void processScroll(double scroll);
  };
}

#endif //MOUSE_H
