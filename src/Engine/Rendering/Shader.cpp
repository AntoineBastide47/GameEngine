//
// Shader.cpp
// Author: Antoine Bastide
// Date: 10/11/2024
//

#include <iostream>

#include "Engine/Rendering/Shader.hpp"
#include "Engine/Log.hpp"

namespace Engine::Rendering {
  Shader &Shader::Use() {
    glUseProgram(this->id);
    return *this;
  }

  void Shader::compile(const char *vertexSource, const char *fragmentSource, const char *geometrySource) {
    uint gShader = 0;

    // Create the vertex shader
    const uint sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vertexSource, nullptr);
    glCompileShader(sVertex);
    checkCompileErrors(sVertex, "VERTEX");

    // Create the fragment shader
    const uint sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fragmentSource, nullptr);
    glCompileShader(sFragment);
    checkCompileErrors(sFragment, "FRAGMENT");

    // Create the geometry shader if there is one
    if (geometrySource) {
      gShader = glCreateShader(GL_GEOMETRY_SHADER);
      glShaderSource(gShader, 1, &geometrySource, nullptr);
      glCompileShader(gShader);
      checkCompileErrors(gShader, "GEOMETRY");
    }

    // Build the shader and link it
    this->id = glCreateProgram();
    glAttachShader(this->id, sVertex);
    glAttachShader(this->id, sFragment);
    if (geometrySource)
      glAttachShader(this->id, gShader);
    glLinkProgram(this->id);
    checkCompileErrors(this->id, "PROGRAM");

    // Delete the pieces of the shader that won't be used anymore
    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
    if (geometrySource)
      glDeleteShader(gShader);
  }

  void Shader::SetFloat(const char *name, const float value, const bool useShader) {
    if (useShader)
      this->Use();
    glUniform1f(glGetUniformLocation(this->id, name), value);
  }

  void Shader::SetInteger(const char *name, const int value, const bool useShader) {
    if (useShader)
      this->Use();
    glUniform1i(glGetUniformLocation(this->id, name), value);
  }

  void Shader::SetVector2f(const char *name, const float x, const float y, const bool useShader) {
    if (useShader)
      this->Use();
    glUniform2f(glGetUniformLocation(this->id, name), x, y);
  }

  void Shader::SetVector2f(const char *name, const glm::vec2 &value, const bool useShader) {
    if (useShader)
      this->Use();
    glUniform2f(glGetUniformLocation(this->id, name), value.x, value.y);
  }

  void Shader::SetVector3f(const char *name, const float x, const float y, const float z, const bool useShader) {
    if (useShader)
      this->Use();
    glUniform3f(glGetUniformLocation(this->id, name), x, y, z);
  }

  void Shader::SetVector3f(const char *name, const glm::vec3 &value, const bool useShader) {
    if (useShader)
      this->Use();
    glUniform3f(glGetUniformLocation(this->id, name), value.x, value.y, value.z);
  }

  void Shader::SetVector4f(
    const char *name, const float x, const float y, const float z, const float w, const bool useShader
  ) {
    if (useShader)
      this->Use();
    glUniform4f(glGetUniformLocation(this->id, name), x, y, z, w);
  }

  void Shader::SetVector4f(const char *name, const glm::vec4 &value, const bool useShader) {
    if (useShader)
      this->Use();
    glUniform4f(glGetUniformLocation(this->id, name), value.x, value.y, value.z, value.w);
  }

  void Shader::SetMatrix4(const char *name, const glm::mat4 &matrix, const bool useShader) {
    if (useShader)
      this->Use();
    glUniformMatrix4fv(glGetUniformLocation(this->id, name), 1, false, value_ptr(matrix));
  }

  void Shader::clear() {
    if (this->id > 0) {
      glDeleteProgram(this->id);
      id = 0;
    }
  }

  void Shader::checkCompileErrors(const uint object, const std::string &type) {
    int success;
    char infoLog[1024];
    if (type != "PROGRAM") {
      glGetShaderiv(object, GL_COMPILE_STATUS, &success);
      if (!success) {
        glGetShaderInfoLog(object, 1024, nullptr, infoLog);
        std::cerr << "| ERROR::SHADER: Compile-time error: Type: " << type << "\n"
          << infoLog << "\n -- --------------------------------------------------- -- "
          << std::endl;
      }
    } else {
      glGetProgramiv(object, GL_LINK_STATUS, &success);
      if (!success) {
        glGetProgramInfoLog(object, 1024, nullptr, infoLog);
        std::cerr << "| ERROR::Shader: Link-time error: Type: " << type << "\n"
          << infoLog << "\n -- --------------------------------------------------- -- "
          << std::endl;
      }
    }
  }
}
