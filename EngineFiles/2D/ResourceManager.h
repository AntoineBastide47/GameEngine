//
// ResourceManager.h
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <map>

#include "Rendering/Shader.h"
#include "Rendering/Texture2D.h"

using Engine2D::Rendering::Shader;
using Engine2D::Rendering::Texture2D;

namespace Engine2D {
  /** ResourceManager is a static class that keeps track of all the shaders and textures that have been loaded to the game. */
  class ResourceManager {
    public:
      /**
       * Loads a shader to memory with the given OpenGL source shader files
       * @param vShaderFile The vertex shader file path
       * @param fShaderFile The fragment shader file path
       * @param gShaderFile The geometry shader file path (Optional)
       * @param name The name of this shader
       * @note Calling this method will log an error if:
       * - parameters vShaderFile, fShaderFile or name are empty, or if a shader with the given name already exists
       * - the files can't be read or are empty
       * @return The loaded shader
       */
      static Shader *LoadShader(
        const std::string &vShaderFile, const std::string &fShaderFile, const std::string &gShaderFile,
        const std::string &name
      );
      /**
       * Finds and returns the shader with the given name
       * @param name The name of the shader
       * @note Calling this method will log an error if the shader with the given name does not exist
       */
      static Shader *GetShader(const std::string &name);
      /**
       * Loads a texture to memory
       * @param filePath The texture file path
       * @param alpha If the texture has an alpha channel
       * @param name The name of this texture
       * @note Calling this method will log an error if:
       * - parameters file or name are empty, or if a texture with the given name already exists
       * - the file can't be read or is empty
       * @return The loaded texture
       */
      static Texture2D *LoadTexture(const std::string &filePath, bool alpha, const std::string &name);
      /**
       * Finds and returns the texture with the given name
       * @param name
       * @note Calling this method will log an error if the texture with the given name does not exist
       */
      static Texture2D *GetTexture(const std::string &name);
      /// Deletes all the loaded textures and shaders
      static void Clear();
    private:
      /// The shaders that have been loaded to memory by the Resource Manager
      static std::map<std::string, Shader *> shaders;
      /// The textures that have been loaded to memory by the Resource Manager
      static std::map<std::string, Texture2D *> textures;
      ResourceManager() = default;
  };
}

#endif //RESOURCEMANAGER_H
