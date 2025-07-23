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
#include "Engine/Macros/Assert.hpp"
#include "Engine/Rendering/ShaderPreProcessor.hpp"
#include "Engine2D/Rendering/Camera2D.hpp"
#include "Engine2D/Rendering/Renderer2D.hpp"
#include "Engine2D/Rendering/Sprite.hpp"

using Engine2D::Game2D;

namespace Engine {
  Shader *ResourceManager::LoadShader(const std::string &name, const std::string &filePath) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    if (filePath.empty())
      return Log::Error("Shader file path is required to load a shader");
    if (name.empty())
      return Log::Error("Shader name is required to load a shader");
    if (shaders.contains(name))
      return shaders.at(name).get();

    // Load the whole shader file
    const cmrc::file shaderFile = Game2D::instance->loadResource(filePath);
    const std::string shaderSource(shaderFile.begin(), shaderFile.end());
    if (shaderSource.empty())
      return Log::Error("Shader source is empty");

    // Split shader stages
    const auto &[vertexCode, fragmentCode, geometryCode] = Rendering::ShaderPreProcessor::preprocess(shaderSource);

    // Validate stages
    if (vertexCode.empty())
      return Log::Error("Vertex shader stage missing");
    if (fragmentCode.empty())
      return Log::Error("Fragment shader stage missing");

    // Compile shader
    auto shader = std::make_unique<Shader>();
    shader->compile(
      vertexCode.c_str(), fragmentCode.c_str(),
      geometryCode.empty() ? nullptr : geometryCode.c_str()
    );
    shader->use();

    // Bind the engine data to the shader
    if (const GLuint blockIndex = glGetUniformBlockIndex(shader->Id(), "Matrices"); blockIndex != GL_INVALID_INDEX)
      glUniformBlockBinding(shader->Id(), blockIndex, Engine2D::Rendering::Camera2D::ENGINE_DATA_BINDING_PORT);

    // Tell the shader how many textures it can hold
    if (const GLint texturesLoc = glGetUniformLocation(shader->Id(), "textures"); texturesLoc != -1) {
      int locations[Engine2D::Rendering::Renderer2D::MAX_TEXTURES];
      for (int i = 0; i < Engine2D::Rendering::Renderer2D::MAX_TEXTURES; ++i)
        locations[i] = i;
      glUniform1iv(texturesLoc, Engine2D::Rendering::Renderer2D::MAX_TEXTURES, locations);
    }

    shaders.emplace(name, std::move(shader));
    return shaders.at(name).get();
  }

  Shader *ResourceManager::GetShader(const std::string &name) {
    if (shaders.contains(name))
      return shaders.at(name).get();
    return Log::Error("Unknown shader: " + name);
  }

  Shader *ResourceManager::GetShaderById(const uint id) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSubSystem);

    for (const auto &val: shaders | std::views::values)
      if (val->id == id)
        return val.get();
    return Log::Error("Unknown shader with id: " + std::to_string(id));
  }

  Texture *ResourceManager::LoadTexture2D(
    const std::string &filePath, const std::string &name, const bool blend
  ) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    if (filePath.empty())
      return Log::Error("File path is required to load a texture");
    if (name.empty())
      return Log::Error("Name is required to load a texture");
    if (textures.contains(name))
      return textures.at(name).get();

    // Load the image from cmrc
    const auto file = Game2D::instance->loadResource(filePath);
    std::string content(file.begin(), file.end());

    if (content.empty())
      return Log::Error("File is empty");

    // Load the data of the image
    int width, height, nrChannels;
    unsigned char *data = stbi_load_from_memory(
      reinterpret_cast<unsigned char *>(content.data()),
      static_cast<int>(content.size()), &width, &height, &nrChannels, 0
    );
    if (!data) {
      stbi_image_free(data);
      return Log::Error("Failed to load image: " + filePath + "\nReason: " + stbi_failure_reason());
    }

    bool transparent = false;
    int internalFormat = 0, dataFormat = 0;
    if (nrChannels == 1) {
      internalFormat = GL_R8;
      dataFormat = GL_RED;
    } else if (nrChannels == 2) {
      internalFormat = GL_RG8;
      dataFormat = GL_RG;
    } else if (nrChannels == 3) {
      internalFormat = GL_RGB8;
      dataFormat = GL_RGB;
    } else if (nrChannels == 4) {
      internalFormat = GL_RGBA8;
      dataFormat = GL_RGBA;

      for (int i = 0; i < width * height; ++i) {
        if (data[i * 4 + 3] < 255) {
          transparent = true;
          break;
        }
      }
    }

    ENGINE_ASSERT(internalFormat & dataFormat, "Image format is not supported");

    // Generate the texture and free the image data
    auto texture = std::make_unique<Texture>(filePath);
    texture->generate(width, height, data, internalFormat, dataFormat, transparent, blend);
    stbi_image_free(data);
    textures.emplace(name, std::move(texture));
    return textures.at(name).get();
  }

  Texture *ResourceManager::GetTexture2D(const std::string &name) {
    if (textures.contains(name))
      return textures.at(name).get();
    return Log::Error("Unknown texture: " + name);
  }

  Texture *ResourceManager::GetTexture2DById(const uint id) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    for (const auto &val: textures | std::views::values)
      if (val->id == id)
        return val.get();
    return Log::Error("Unknown texture with id: " + std::to_string(id));
  }

  Sprite *ResourceManager::GetSprite(const std::string &name) {
    if (sprites.contains(name))
      return sprites.at(name).get();
    return nullptr;
  }

  std::pair<Texture *, Sprite *> ResourceManager::LoadTexture2DAndSprite(
    const std::string &filePath, const std::string &name, const glm::vec<4, float> &rect,
    const bool blend
  ) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    auto texture = LoadTexture2D(filePath, name, blend);
    auto sprite = CreateSpriteFromTexture(name, rect);
    return {texture, sprite};
  }

  Sprite *ResourceManager::CreateSpriteFromTexture(
    const std::string &textureName, const glm::vec<4, float> &rect
  ) {
    if (!textures.contains(textureName))
      return Log::Error("Texture not found: " + textureName);

    const auto &texture = GetTexture2D(textureName);
    auto sprite = std::make_unique<Sprite>();
    sprite->texture = texture;
    sprite->transparent = texture->transparent;
    sprite->rect = rect;
    sprites.emplace(textureName, std::move(sprite));
    return sprites.at(textureName).get();
  }

  Sprite *ResourceManager::CreateSprite(
    const std::string &spriteName, const std::string &textureName, const glm::vec<4, float> &rect
  ) {
    if (!textures.contains(textureName))
      return Log::Error("Texture not found: " + textureName);
    if (sprites.contains(spriteName))
      return Log::Error("Sprite already exists: " + spriteName);

    const auto &texture = GetTexture2D(textureName);
    auto sprite = std::make_unique<Sprite>();
    sprite->texture = texture;
    sprite->rect = rect;
    sprite->transparent = texture->transparent;
    sprites.emplace(spriteName, std::move(sprite));
    return sprites.at(spriteName).get();
  }

  void ResourceManager::Clear() {
    for (const auto &shader: shaders | std::views::values)
      shader->clear();
    shaders.clear();
    for (const auto &texture: textures | std::views::values)
      texture->clear();
    textures.clear();
  }
}
