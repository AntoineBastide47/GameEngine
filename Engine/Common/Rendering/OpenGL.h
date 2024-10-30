//
// OpenGL.h
// OpenGL: Implementation of the OpenGL core functionnalities
// Author: Antoine Bastide
// Date: 30/10/2024
//

#ifndef OPENGL_H
#define OPENGL_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace Engine::Rendering {
  class OpenGL {
    public:
      static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
  };
};

#endif //OPENGL_H
