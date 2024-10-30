//
// Game2D.cpp
// Game2D: 
// Author: Antoine Bastide
// Date: 30/10/2024
//

#include "Game2D.h"

#include <iostream>

using Engine::Rendering::OpenGL;

namespace Engine2D {
  Game2D::Game2D(const int width, const int height, char *title) : width(width), height(height), title(title), window(nullptr) {}
  Game2D::~Game2D() {
    glfwDestroyWindow(window);
    window = nullptr;
    glfwTerminate();
  }

  int Game2D::Initialize() {
    // Initialize and Configure GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // Create the window
    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr)
    {
      std::cout << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, OpenGL::framebuffer_size_callback);

    // Initialize GLEW
    if (const GLenum err = glewInit(); err != GLEW_OK) {
      std::cerr << "Error initializing GLEW: " << glewGetErrorString(err) << std::endl;
      exit(EXIT_FAILURE);
    }

    // Render Loop
    while (!glfwWindowShouldClose(window))
    {
      this->ProcessInput();

      glfwSwapBuffers(window);
      glfwPollEvents();
    }

    this->Quit();
    return 0;
  }

  void Game2D::ProcessInput() const {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);
  }

  void Game2D::Quit() {
    this->~Game2D();
  }


}
