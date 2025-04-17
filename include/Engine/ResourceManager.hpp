//
// ResourceManager.h
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <map>
#include <glm/glm.hpp>

namespace Engine::Rendering {
  class Shader;
  class Texture;
}
namespace Engine2D::Rendering {
  class Sprite;
}

using Engine::Rendering::Shader;
using Engine::Rendering::Texture;
using Engine2D::Rendering::Sprite;

namespace Engine {
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
      static std::shared_ptr<Shader> LoadShader(
        const std::string &vShaderFile, const std::string &fShaderFile, const std::string &gShaderFile,
        const std::string &name
      );

      /**
       * Finds and returns the shader with the given name
       * @param name The name of the shader
       * @note Calling this method will log an error if the shader with the given name does not exist
       */
      static std::shared_ptr<Shader> GetShader(const std::string &name);

      /**
       * Finds and returns the shader with the given od
       * @param id The id of the shader
       * @note Calling this method will log an error if the shader with the given name does not exist
       */
      static std::shared_ptr<Shader> GetShaderById(uint id);

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
      static std::shared_ptr<Texture> LoadTexture2D(const std::string &filePath, bool alpha, const std::string &name);

      /**
       * Finds and returns the texture with the given name
       * @param name The name of the texture
       * @note Calling this method will log an error if the texture with the given name does not exist
       */
      static std::shared_ptr<Texture> GetTexture2D(const std::string &name);

      /**
       * Finds and returns the texture with the given name
       * @param id The id of the texture
       * @note Calling this method will log an error if the texture with the given name does not exist
       */
      static std::shared_ptr<Texture> GetTexture2DById(uint id);

      /**
       * Creates a sprite using the texture with the given name
       * @param textureName The name of the texture to base this sprite on
       * @note Log's an error if no texture with the given name was found
       */
      static std::shared_ptr<Sprite> CreateSpriteFromTexture(const std::string &textureName);

      /**
       * Creates a sprite using the texture with the given name
       * @param spriteName The name of the new sprite
       * @param textureName The name of the texture to base this sprite on
       * @param rect The area of the texture to use to represent the sprite
       * @note Log's an error if no texture with the given name was found
       */
      static std::shared_ptr<Sprite> CreateSprite(
        const std::string &spriteName, const std::string &textureName, const glm::vec4 &rect = glm::vec4(0, 0, 1, 1)
      );

      /**
       * Finds or create the sprite with the given name and returns it
       * @param name The name of the sprite, must be the same as the loaded textures name
       * @note Log's an error if no texture with the given name was found
       */
      static std::shared_ptr<Sprite> GetSprite(const std::string &name);

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
      static std::pair<std::shared_ptr<Texture>, std::shared_ptr<Sprite>> LoadTexture2DAndSprite(
        const std::string &filePath, bool alpha, const std::string &name
      );

      /// Deletes all the loaded textures and shaders
      static void Clear();
    private:
      /// The shaders that have been loaded to memory by the Resource Manager
      static std::map<std::string, std::shared_ptr<Shader>> shaders;
      /// The textures that have been loaded to memory by the Resource Manager
      static std::map<std::string, std::shared_ptr<Texture>> textures;
      /// The sprites that have been loaded to memory by the Resource Manager
      static std::map<std::string, std::shared_ptr<Sprite>> sprites;
      ResourceManager() = default;
  };
}

#endif //RESOURCEMANAGER_H
