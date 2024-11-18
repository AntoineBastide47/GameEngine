//
// Mouse.h
// Author: Antoine Bastide
// Date: 18/11/2024
//

#ifndef MOUSE_H
#define MOUSE_H

#include "2D/Types/Vector2.h"
#include "Common/Event.h"
#include "Common/RenderingHeaders.h"

namespace Engine2D {
  class Game2D;
}

namespace Engine::Input {
  /** The MouseContext struct provides a snapshot of the state of mouse buttons: pressed, help and release states. */
  struct MouseContext {
    /** Set to true when a mouse button is initially pressed. */
    bool pressed: 1;
    /** Set to true if a mouse button is actively pressed down, remaining true as long as the key is held. */
    bool held: 1;
    /** Set to true when a mouse button is released after being pressed. */
    bool released: 1;
  };
  /** The MouseScroll struct provides a snapshot of the state of the mouse scroll: up and down. */
  struct MouseScroll {
    /** Set to true when scrolling upwards. */
    bool up: 1;
    /** Set to true when scrolling downwards. */
    bool down: 1;
  };

  class MouseContextEvent : public Event<MouseContext> {
    friend class Mouse;
  };
  class MousePositionEvent : public Event<Engine2D::Vector2> {
    friend class Mouse;
  };
  class MouseScrollEvent : public Event<MouseScroll> {
    friend class Mouse;
  };

  /**
   * The Keyboard class manages Mouse events, such as mouse button presses and releases, mouse position and scroll,
   * and provides a system for registering callbacks to respond to these events.
   * Each key has an associated `KeyboardEvent` that can be
   * used to trigger actions when that key is pressed, held, or released.
   */
  class Mouse {
    public:
      static MouseContextEvent LEFT, MIDDLE, RIGHT, BUTTON_4, BUTTON_5, BUTTON_6, BUTTON_7, BUTTON_8;
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
      static bool previousKeyStates[GLFW_MOUSE_BUTTON_8 + 1];
      /** The window pointer required for querying mouse states. */
      static GLFWwindow *window;
      /** Temporary reference to the x position of the mouse */
      static double x;
      /** Temporary reference to the y position of the mouse */
      static double y;

      Mouse() = default;
      ~Mouse() = default;

      /** Initializes the Mouse instance */
      static void initialize(GLFWwindow *window);
      /** Processes all the inputs of the current frame of the game and calls the corresponding key events. */
      static void processInput();
      /** Processes an individual mouse button */
      static void processButton(int keyCode, MouseContextEvent *event);
      /** Processes the movement of the mouse */
      static void processPosition();
      /** Processes the scrolling of the mouse */
      static void processScroll(double scroll);
  };
}

#endif //MOUSE_H
