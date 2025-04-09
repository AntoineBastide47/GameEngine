//
// ResourceManager.cpp
// Author: Antoine Bastide
// Date: 10/11/2024
//

#include <ranges>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "Engine2D/Rendering/stb_image.hpp"

#include "Engine/Rendering/Shader.hpp"
#include "Engine/Rendering/Texture.hpp"
#include "Engine/ResourceManager.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine/Log.hpp"
#include "Engine2D/Rendering/Sprite.hpp"

using Engine2D::Game2D;

namespace Engine {
  std::map<std::string, std::shared_ptr<Texture>> ResourceManager::textures;
  std::map<std::string, std::shared_ptr<Shader>> ResourceManager::shaders;
  std::map<std::string, std::shared_ptr<Sprite>> ResourceManager::sprites;

  std::shared_ptr<Shader> ResourceManager::LoadShader(
    const std::string &vShaderFile, const std::string &fShaderFile, const std::string &gShaderFile,
    const std::string &name
  ) {
    if (vShaderFile.empty())
      return Engine::Log::Error("Vertex shader file path is required to load a shader");
    if (fShaderFile.empty())
      return Engine::Log::Error("Fragment shader file path is required to load a shader");
    if (name.empty())
      return Engine::Log::Error("Shader name is required to load a shader");
    if (shaders.contains(name))
      return shaders[name];

    std::string geometryCode;
    // Load vertex shader
    const cmrc::file vertexShaderFile = Game2D::instance->loadResource(vShaderFile);
    const auto vertexCode = std::string(vertexShaderFile.begin(), vertexShaderFile.end());
    if (vertexCode.empty())
      return Engine::Log::Error("Vertex Shader is empty");

    // Load fragment shader
    const cmrc::file fragmentShaderFile = Game2D::instance->loadResource(fShaderFile);
    const auto fragmentCode = std::string(fragmentShaderFile.begin(), fragmentShaderFile.end());
    if (fragmentCode.empty())
      return Engine::Log::Error("Vertex Shader is empty");

    // Load geometry shader if provided
    if (!gShaderFile.empty()) {
      const cmrc::file geometryShaderFile = Game2D::instance->loadResource(gShaderFile);
      geometryCode = std::string(geometryShaderFile.begin(), geometryShaderFile.end());
      if (geometryCode.empty())
        Engine::Log::Warning("Geometry Shader is empty, it won't be used");
    }

    // Create the shader
    const auto shader = std::make_shared<Shader>();
    shader->compile(
      vertexCode.c_str(), fragmentCode.c_str(),
      !gShaderFile.empty() && !geometryCode.empty() ? geometryCode.c_str() : nullptr
    );
    return shaders[name] = shader;
  }

  std::shared_ptr<Shader> ResourceManager::GetShader(const std::string &name) {
    if (shaders.contains(name))
      return shaders[name];
    return Engine::Log::Error("Unknown shader: " + name);
  }

  std::shared_ptr<Texture> ResourceManager::LoadTexture(
    const std::string &filePath, const bool alpha, const std::string &name
  ) {
    if (filePath.empty())
      return Engine::Log::Error("File path is required to load a texture");
    if (name.empty())
      return Engine::Log::Error("Name is required to load a texture");
    if (textures.contains(name))
      return textures[name];

    // Create the texture
    const auto texture = std::make_shared<Texture>();
    if (alpha) {
      texture->imageFormat = GL_RGBA;
      texture->internalFormat = GL_RGBA;
    }

    // Load the image from cmrc
    const auto file = Game2D::instance->loadResource(filePath);
    std::string content(file.begin(), file.end());

    if (content.empty())
      return Engine::Log::Error("File is empty");

    // Load the data of the image
    int width, height, nrChannels;
    unsigned char *data = stbi_load_from_memory(
      reinterpret_cast<unsigned char *>(content.data()),
      static_cast<int>(content.size()), &width, &height, &nrChannels, 0
    );
    if (!data) {
      stbi_image_free(data);
      return Engine::Log::Error("Failed to load image: " + filePath + "\nReason: " + stbi_failure_reason());
    }

    // Generate the texture and free the image data
    texture->generate(width, height, data);
    stbi_image_free(data);
    return textures[name] = texture;
  }

  std::shared_ptr<Texture> ResourceManager::GetTexture(const std::string &name) {
    if (textures.contains(name))
      return textures[name];
    return Engine::Log::Error("Unknown texture: " + name);
  }

  std::shared_ptr<Sprite> ResourceManager::GetSprite(const std::string &name) {
    if (sprites.contains(name))
      return sprites[name];
    return CreateSprite(name);
  }

  std::pair<std::shared_ptr<Texture>, std::shared_ptr<Sprite>> ResourceManager::LoadTextureAndSprite(
    const std::string &filePath, const bool alpha, const std::string &name
  ) {
    auto texture = LoadTexture(filePath, alpha, name);
    auto sprite = CreateSprite(name);
    return {texture, sprite};
  }

  std::shared_ptr<Sprite> ResourceManager::CreateSprite(const std::string &textureName) {
    if (!textures.contains(textureName))
      return Engine::Log::Error("Texture not found: " + textureName);

    const auto &texture = textures[textureName];

    auto sprite = std::make_shared<Sprite>();
    sprite->texture = texture;

    return sprites[textureName] = sprite;
  }

  void ResourceManager::Clear() {
    for (const auto shader: shaders | std::views::values)
      shader->clear();
    shaders.clear();
    for (const auto texture: textures | std::views::values)
      texture->clear();
    textures.clear();
  }
}
