//
// Shader.h
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef SHADER_H
#define SHADER_H

#include "Engine/RenderingHeaders.hpp"

namespace Engine2D {
  class ResourceManager;
}

namespace Engine2D::Rendering {
  /// Represents an OpenGL shader program, managing shader compilation, linking, and uniform settings.
  class Shader {
    friend class Engine2D::ResourceManager;
    public:
      /// Used to reference and manage this shader program within OpenGL.
      uint id;

      Shader() = default;

      /**
       * Sets the current shader as active and returns a reference to this Shader object, enabling method chaining.
       * @return Reference to this Shader object.
       */
      Shader &Use();

      /**
       * Sets a float uniform in the shader program.
       * @param name Name of the uniform variable in the shader.
       * @param value Float value to set.
       * @param useShader Whether to activate the shader before setting the uniform (default is false).
       */
      void SetFloat(const char *name, float value, bool useShader = false);

      /**
       * Sets an integer uniform in the shader program.
       * @param name Name of the uniform variable in the shader.
       * @param value Integer value to set.
       * @param useShader Whether to activate the shader before setting the uniform (default is false).
       */
      void SetInteger(const char *name, int value, bool useShader = false);

      /**
       * Sets a 2D vector uniform in the shader program.
       * @param name Name of the uniform variable in the shader.
       * @param x X component of the vector.
       * @param y Y component of the vector.
       * @param useShader Whether to activate the shader before setting the uniform (default is false).
       */
      void SetVector2f(const char *name, float x, float y, bool useShader = false);

      /**
       * Sets a 2D vector uniform in the shader program.
       * @param name Name of the uniform variable in the shader.
       * @param value 2D vector value to set.
       * @param useShader Whether to activate the shader before setting the uniform (default is false).
       */
      void SetVector2f(const char *name, const glm::vec2 &value, bool useShader = false);

      /**
       * Sets a 3D vector uniform in the shader program.
       * @param name Name of the uniform variable in the shader.
       * @param x X component of the vector.
       * @param y Y component of the vector.
       * @param z Z component of the vector.
       * @param useShader Whether to activate the shader before setting the uniform (default is false).
       */
      void SetVector3f(const char *name, float x, float y, float z, bool useShader = false);

      /**
       * Sets a 3D vector uniform in the shader program.
       * @param name Name of the uniform variable in the shader.
       * @param value 3D vector value to set.
       * @param useShader Whether to activate the shader before setting the uniform (default is false).
       */
      void SetVector3f(const char *name, const glm::vec3 &value, bool useShader = false);

      /**
       * Sets a 4D vector uniform in the shader program.
       * @param name Name of the uniform variable in the shader.
       * @param x X component of the vector.
       * @param y Y component of the vector.
       * @param z Z component of the vector.
       * @param w W component of the vector.
       * @param useShader Whether to activate the shader before setting the uniform (default is false).
       */
      void SetVector4f(const char *name, float x, float y, float z, float w, bool useShader = false);

      /**
       * Sets a 4D vector uniform in the shader program.
       * @param name Name of the uniform variable in the shader.
       * @param value 4D vector value to set.
       * @param useShader Whether to activate the shader before setting the uniform (default is false).
       */
      void SetVector4f(const char *name, const glm::vec4 &value, bool useShader = false);

      /**
       * Sets a 4x4 matrix uniform in the shader program.
       * @param name Name of the uniform variable in the shader.
       * @param matrix 4x4 matrix value to set.
       * @param useShader Whether to activate the shader before setting the uniform (default is false).
       */
      void SetMatrix4(const char *name, const glm::mat4 &matrix, bool useShader = false);
    private:
      /**
       * Compiles the shader program from provided source code.
       * Supports vertex, fragment, and optional geometry shaders, compiling each
       * and linking them into a single program.
       * @param vertexSource Source code for the vertex shader.
       * @param fragmentSource Source code for the fragment shader.
       * @param geometrySource (Optional) Source code for the geometry shader.
       */
      void compile(const char *vertexSource, const char *fragmentSource, const char *geometrySource = nullptr);
      /** Clears the shader program, releasing associated resources. */
      void clear();
      /// Checks for compilation or linking errors in shader objects.
      static void checkCompileErrors(uint object, const std::string &type);
  };
}

#endif // SHADER_H
