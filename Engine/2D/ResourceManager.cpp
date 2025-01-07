//
// ResourceManager.cpp
// Author: Antoine Bastide
// Date: 10/11/2024
//

#include <fstream>
#include <ranges>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include "2D/Rendering/stb_image.h"

#include "2D/ResourceManager.h"
#include "2D/Game2D.h"
#include "Common/Log.h"

namespace Engine2D {
  std::map<std::string, std::shared_ptr<Texture2D>> ResourceManager::textures;
  std::map<std::string, std::shared_ptr<Shader>> ResourceManager::shaders;

  std::shared_ptr<Shader> ResourceManager::LoadShader(
    const std::string &vShaderFile, const std::string &fShaderFile, const std::string &gShaderFile,
    const std::string &name
  ) {
    if (vShaderFile.empty())
      return LOG_ERROR("Vertex shader file path is required to load a shader");
    if (fShaderFile.empty())
      return LOG_ERROR("Fragment shader file path is required to load a shader");
    if (name.empty())
      return LOG_ERROR("Shader name is required to load a shader");
    if (shaders.contains(name))
      return shaders[name];

    std::string geometryCode;
    // Load vertex shader
    const cmrc::file vertexShaderFile = Game2D::instance->loadResource(vShaderFile);
    const auto vertexCode = std::string(vertexShaderFile.begin(), vertexShaderFile.end());
    if (vertexCode.empty())
      return LOG_ERROR("Vertex Shader is empty");

    // Load fragment shader
    const cmrc::file fragmentShaderFile = Game2D::instance->loadResource(fShaderFile);
    const auto fragmentCode = std::string(fragmentShaderFile.begin(), fragmentShaderFile.end());
    if (fragmentCode.empty())
      return LOG_ERROR("Vertex Shader is empty");

    // Load geometry shader if provided
    if (!gShaderFile.empty()) {
      const cmrc::file geometryShaderFile = Game2D::instance->loadResource(gShaderFile);
      geometryCode = std::string(geometryShaderFile.begin(), geometryShaderFile.end());
      if (geometryCode.empty())
        LOG_WARNING("Geometry Shader is empty, it won't be used");
    }

    // Create the shader
    const auto shader = std::make_shared<Shader>();
    shader->Compile(
      vertexCode.c_str(), fragmentCode.c_str(),
      !gShaderFile.empty() && !geometryCode.empty() ? geometryCode.c_str() : nullptr
    );
    return shaders[name] = shader;
  }

  std::shared_ptr<Shader> ResourceManager::GetShader(const std::string &name) {
    if (const auto it = shaders.find(name); it != shaders.end())
      return it->second;
    return LOG_ERROR("Unknown shader: " + name);
  }

  std::shared_ptr<Texture2D> ResourceManager::LoadTexture(
    const std::string &filePath, const bool alpha, const std::string &name
  ) {
    if (filePath.empty())
      return LOG_ERROR("File path is required to load a texture");
    if (name.empty())
      return LOG_ERROR("Name is required to load a texture");
    if (textures.contains(name))
      return textures[name];

    // Create the texture
    const auto texture = std::make_shared<Texture2D>();
    if (alpha) {
      texture->imageFormat = GL_RGBA;
      texture->internalFormat = GL_RGBA;
    }

    // Load the image from cmrc
    const auto file = Game2D::instance->loadResource(filePath);
    std::string content(file.begin(), file.end());

    if (content.empty())
      return LOG_ERROR("File is empty");

    // Load the data of the image
    int width, height, nrChannels;
    unsigned char *data = stbi_load_from_memory(
      reinterpret_cast<unsigned char *>(content.data()),
      static_cast<int>(content.size()), &width, &height, &nrChannels, 0
    );
    if (!data) {
      stbi_image_free(data);
      return LOG_ERROR("Failed to load image: " + filePath + "\nReason: " + stbi_failure_reason());
    }

    // Generate the texture and free the image data
    texture->Generate(width, height, data);
    stbi_image_free(data);
    return textures[name] = texture;
  }

  std::shared_ptr<Texture2D> ResourceManager::GetTexture(const std::string &name) {
    if (const auto it = textures.find(name); it != textures.end())
      return it->second;
    return LOG_ERROR("Unknown texture: " + name);
  }

  void ResourceManager::Clear() {
    for (const auto shader: shaders | std::views::values)
      shader->Clear();
    shaders.clear();
    for (const auto texture: textures | std::views::values)
      texture->clear();
    textures.clear();
  }
}
