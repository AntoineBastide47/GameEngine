//
// ResourceManager.cpp
// ResourceManager: 
// Author: Antoine Bastide
// Date: 10/11/2024
//

#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <ranges>

#include "2D/ResourceManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "2D/Rendering/stb_image.h"

namespace Engine2D {
  std::map<std::string, Texture2D> ResourceManager::textures;
  std::map<std::string, Shader> ResourceManager::shaders;

  Shader ResourceManager::LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile,
                                                const std::string &name) {
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    try {
      // open files
      std::ifstream vertexShaderFile(vShaderFile);
      std::ifstream fragmentShaderFile(fShaderFile);
      std::stringstream vShaderStream, fShaderStream;

      // read file's buffer contents into streams
      vShaderStream << vertexShaderFile.rdbuf();
      fShaderStream << fragmentShaderFile.rdbuf();

      // close file handlers
      vertexShaderFile.close();
      fragmentShaderFile.close();

      // convert stream into string
      vertexCode = vShaderStream.str();
      fragmentCode = fShaderStream.str();

      // if geometry shader path is present, also load a geometry shader
      if (gShaderFile != nullptr) {
        std::ifstream geometryShaderFile(gShaderFile);
        std::stringstream gShaderStream;
        gShaderStream << geometryShaderFile.rdbuf();
        geometryShaderFile.close();
        geometryCode = gShaderStream.str();
      }
    }
    catch (std::exception& e) {
      throw std::runtime_error("ERROR::SHADER: Failed to read shader files\n" + std::string(e.what()));
    }

    // Create the shader
    Shader shader{};
    shader.Compile(vertexCode.c_str(), fragmentCode.c_str(), gShaderFile ? geometryCode.c_str() : nullptr);
    return shaders[name] = shader;
  }

  Shader ResourceManager::GetShader(const std::string &name) {
    if (const auto it = shaders.find(name); it != shaders.end())
      return it->second;
    throw std::runtime_error("ERROR::RESOURCES: unknown shader: " + name);
  }

  Texture2D ResourceManager::LoadTexture(const char *file, const bool alpha, const std::string &name) {
    // Create the texture
    Texture2D texture;
    if (alpha) {
      texture.imageFormat = GL_RGBA;
      texture.internalFormat = GL_RGBA;
    }

    // Load the image
    int width, height, nrChannels;
    unsigned char *data = stbi_load(file, &width, &height, &nrChannels, 0);
    if (!data)
      throw std::runtime_error("ERROR::TEXTURE2D: Failed to load image: " + std::string(file));

    // Generate the texture and free the image data
    texture.Generate(width, height, data);
    stbi_image_free(data);
    return textures[name] = texture;
  }

  Texture2D ResourceManager::GetTexture(const std::string &name) {
    if (const auto it = textures.find(name); it != textures.end())
      return it->second;
    throw std::runtime_error("ERROR::TEXTURE2D: unknown texture: " + name);
  }

  void ResourceManager::Clear() {
    for (auto &shader: shaders | std::views::values)
      shader.Clear();
    for (auto &texture: textures | std::views::values)
      texture.Clear();
  }
}
