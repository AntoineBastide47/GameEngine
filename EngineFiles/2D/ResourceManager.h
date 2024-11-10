//
// ResourceManager.h
// ResourceManager: 
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
  class ResourceManager {
    public:
      static std::map<std::string, Shader> shaders;
      static std::map<std::string, Texture2D> textures;
      static Shader LoadShader(const char *vShaderFile, const char *fShaderFile, const char *gShaderFile, const std::string &name);
      static Shader GetShader(const std::string &name);
      static Texture2D LoadTexture(const char *file, bool alpha, const std::string &name);
      static Texture2D GetTexture(const std::string &name);
      static void Clear();
    private:
      ResourceManager() = default;
  };
}

#endif //RESOURCEMANAGER_H
