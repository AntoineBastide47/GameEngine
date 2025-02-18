//
// Mouse.h
// Author: Antoine Bastide
// Date: 18/11/2024
//

#ifndef MOUSE_H
#define MOUSE_H

#include <bitset>
#include <glm/glm.hpp>

#include "Engine/Event.h"
#include "Engine/RenderingHeaders.h"
#include "Engine/Input/InputContexts.h"

namespace Engine2D {
  class Game2D;
}

namespace Engine::Input {
  class MouseButtonEvent final : public Event<KeyboardAndMouseContext> {
    friend class Mouse;
  };
  class MousePositionEvent final : public Event<glm::vec2> {
    friend class Mouse;
  };
  class MouseScrollEvent final : public Event<MouseScroll> {
    friend class Mouse;
  };

  /**
   * The Mouse class manages mouse events, such as mouse button presses and releases, mouse position and scroll,
   * and provides a system for registering callbacks to respond to these events.
   * The mouse position, scroll and button all have an associated Event that triggers when they are updated.
   */
  class Mouse final {
    public:
      static MouseButtonEvent LEFT, MIDDLE, RIGHT, BUTTON_4, BUTTON_5, BUTTON_6, BUTTON_7, BUTTON_8;
      static MouseScrollEvent SCROLL;

      /// Sets the mouse lock state to false and hidden state to false.
      static void SetMouseNormal();
      /// Sets the mouse lock state to false and hidden state to true.
      static void SetMouseHidden();
      /// Sets the mouse lock state to true and hidden state to true.
      static void SetMouseDisabled();
      /// @returns The current position of the mouse on the screen
      [[nodiscard]] static glm::vec2 Position();

      friend class Engine2D::Game2D;
    private:
      /// If any of the mouse buttons were pressed last frame
      static std::bitset<GLFW_MOUSE_BUTTON_LAST + 1> previousKeyStates;
      /// The window pointer required for querying mouse states.
      static GLFWwindow *window;

      Mouse() = default;
      ~Mouse();

      /// Initializes the Mouse instance
      static void initialize(GLFWwindow *window);
      /// Processes all the inputs of the current frame of the game and calls the corresponding key events.
      static void processInput();
      /// Processes an individual mouse button
      static void processButton(int keyCode, MouseButtonEvent *event, KeyboardAndMouseContext &ctx);
      /// Processes the scrolling of the mouse
      static void processScroll(double scroll);
  };
}

#endif //MOUSE_H
