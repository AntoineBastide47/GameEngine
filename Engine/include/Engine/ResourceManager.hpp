//
// ResourceManager.h
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <map>
#include <glm/glm.hpp>

#include "Engine/Rendering/Shader.hpp"
#include "Engine/Rendering/Texture.hpp"
#include "Engine2D/Rendering/Sprite.hpp"

using Engine::Rendering::Shader;
using Engine::Rendering::Texture;
using Engine2D::Rendering::Sprite;

namespace Engine {
  /** ResourceManager is a static class that keeps track of all the shaders and textures that have been loaded to the game. */
  class ResourceManager final {
    public:
      /**
       * Loads a shader to memory with the given OpenGL source shader files
       * @param filePath The vertex shader file path
       * @param name The name of this shader
       * @note Calling this method will log an error if:
       * - parameters vShaderFile, fShaderFile or name are empty, or if a shader with the given name already exists
       * - the files can't be read or are empty
       * @return The loaded shader
       */
      static Shader *LoadShader(const std::string &name, const std::string &filePath);

      /**
       * Finds and returns the shader with the given name
       * @param name The name of the shader
       * @note Calling this method will log an error if the shader with the given name does not exist
       */
      static Shader *GetShader(const std::string &name);

      /**
       * Finds and returns the shader with the given od
       * @param id The id of the shader
       * @note Calling this method will log an error if the shader with the given name does not exist
       */
      static Shader *GetShaderById(uint id);

      /**
       * Loads a texture to memory
       * @param filePath The texture file path
       * @param name The name of this texture
       * @param blend
       * @note Calling this method will log an error if:
       * - parameters file or name are empty, or if a texture with the given name already exists
       * - the file can't be read or is empty
       * @return The loaded texture
       */
      static Texture *LoadTexture2D(const std::string &name, const std::string &filePath, bool blend = false);

      /**
       * Finds and returns the texture with the given name
       * @param name The name of the texture
       * @note Calling this method will log an error if the texture with the given name does not exist
       */
      static Texture *GetTexture2D(const std::string &name);

      /**
       * Finds and returns the texture with the given name
       * @param id The id of the texture
       * @note Calling this method will log an error if the texture with the given name does not exist
       */
      static Texture *GetTexture2DById(uint id);

      /**
       * Creates a sprite using the texture with the given name
       * @param textureName The name of the texture to base this sprite on
       * @param rect The normalized area of the texture to use to represent the sprite in the given format: (u, v, width, height)
       * @note Log's an error if no texture with the given name was found
       */
      static Sprite *CreateSpriteFromTexture(const std::string &textureName, const glm::vec4 &rect = defaultRect);

      /**
       * Creates a sprite using the texture with the given name
       * @param spriteName The name of the new sprite
       * @param textureName The name of the texture to base this sprite on
       * @param rect The normalized area of the texture to use to represent the sprite in the given format: (u, v, width, height)
       * @note Log's an error if no texture with the given name was found
       */
      static Sprite *CreateSprite(
        const std::string &spriteName, const std::string &textureName, const glm::vec4 &rect = defaultRect
      );

      /**
       * Finds or create the sprite with the given name and returns it
       * @param name The name of the sprite
       * @note Log's an error if no texture with the given name was found
       */
      static Sprite *GetSprite(const std::string &name);

      /**
       * Loads a texture to memory
       * @param filePath The texture file path
       * @param name The name of this texture
       * @param rect The normalized area of the texture to use to represent the sprite in the given format: (u, v, width, height)
       * @param blend
       * @note Calling this method will log an error if:
       * - parameters file or name are empty, or if a texture with the given name already exists
       * - the file can't be read or is empty
       * @return The loaded texture
       */
      static std::pair<Texture *, Sprite *> LoadTexture2DAndSprite(
        const std::string &name, const std::string &filePath, const glm::vec4 &rect = defaultRect, bool blend = false
      );

      /// @returns The name of the given shader or "" if it isn't found
      static const std::string &GetShaderName(const Shader *shader);
      /// @returns The name of the given texture 2D or "" if it isn't found
      static const std::string &GetTexture2DName(const Texture *texture);
      /// @returns The name of the given sprite or "" if it isn't found
      static const std::string &GetSpriteName(const Sprite *sprite);

      /// Deletes all the loaded textures and shaders
      static void Clear();
    private:
      /// The shaders that have been loaded to memory by the Resource Manager
      inline static std::map<std::string, std::unique_ptr<Shader>> shaders;
      /// The textures that have been loaded to memory by the Resource Manager
      inline static std::map<std::string, std::unique_ptr<Texture>> textures;
      /// The sprites that have been loaded to memory by the Resource Manager
      inline static std::map<std::string, std::unique_ptr<Sprite>> sprites;
      /// The default value for a rect (the whole image)
      inline static glm::vec4 defaultRect{0, 0, 1, 1};

      ResourceManager() = default;

      static const std::string &EmptyString();
  };
}

#endif //RESOURCEMANAGER_H
