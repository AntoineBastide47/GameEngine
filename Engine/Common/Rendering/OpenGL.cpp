//
// OpenGL.cpp
// OpenGL: Implementation of the OpenGL core functionalities
// Author: Antoine Bastide
// Date: 30/10/2024
//

#include "../../../include/Common/Rendering/OpenGL.h"

namespace Engine::Rendering {
  void OpenGL::framebuffer_size_callback(GLFWwindow* window, const int width, const int height) {
    glViewport(0, 0, width, height);
  }
}