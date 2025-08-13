//
// Window.hpp
// Author: Antoine Bastide
// Date: 03.08.2025
//

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "Engine/RenderingHeaders.hpp"

namespace Editor {
  class Window final {
    friend class LevelEditor;
    public:
      Window();

      /// Start's and Run's the editor
      /// @note Do not call this function yourself in your code, it will be called in the main.cpp of the editor
      void Run();

      /// @returns True if the Window instance has been initialized, False if not
      [[nodiscard]] static bool Initialized();
      /// @returns The width of the window's
      [[nodiscard]] static float WindowWidth();
      /// @returns The height of the window's
      [[nodiscard]] static float WindowHeight();

      /// Quits the editor
      static void Quit();
    private:
      /// The title of the game window
      const char *title;
      /// The width of the game window
      int width;
      /// The height of the game window
      int height;
      /// The pointer to the game window
      GLFWwindow *window;
      /// Whether the mouse left button is currently down
      bool mouseDown;

      inline static Window *instance;

      void initialize();
      void quit();

      static void framebuffer_size_callback(GLFWwindow *window, int width, int height);
      static void window_close_callback(GLFWwindow *window);

      static void window_focus_callback(GLFWwindow *window, int focused);
      static void cursor_enter_callback(GLFWwindow *window, int entered);
      static void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
      static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
      static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
      static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
      static void char_callback(GLFWwindow *window, unsigned int codepoint);
      static void monitor_callback(GLFWwindow *window, int monitor, int width, int height);
  };
} // Editor

#endif //WINDOW_HPP
