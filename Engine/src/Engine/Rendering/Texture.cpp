//
// Texture2D.cpp
// Author: Antoine Bastide
// Date: 10/11/2024
//

#include "Engine/Rendering/Texture.hpp"
#include "Engine/RenderingHeaders.hpp"
#include "Engine/Macros/Profiling.hpp"

namespace Engine::Rendering {
  Texture::Texture()
    : id(0), width(0), height(0), transparent(false) {}

  int Texture::Width() const {
    return width;
  }

  int Texture::Height() const {
    return height;
  }

  unsigned int Texture::Id() const {
    return id;
  }

  const std::string &Texture::Name() const {
    return name;
  }

  const std::string &Texture::Path() const {
    return path;
  }

  bool Texture::Transparent() const {
    return transparent;
  }

  void Texture::generate(
    const int width, const int height, const unsigned char *data, const int internalFormat, const int dataFormat,
    const bool transparent, const bool blended
  ) {
    ENGINE_PROFILE_FUNCTION(Engine::Settings::Profiling::ProfilingLevel::PerSystem);

    this->width = width;
    this->height = height;
    this->transparent = transparent;

    // create Texture
    glGenTextures(1, &this->id);
    glBindTexture(GL_TEXTURE_2D, this->id);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

    // set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, blended ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, blended ? GL_LINEAR : GL_NEAREST);

    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void Texture::clear() {
    if (this->id > 0) {
      glDeleteTextures(1, &this->id);
      id = 0;
    }
  }

  void Texture::bind(const int offset) const {
    glActiveTexture(GL_TEXTURE0 + offset);
    glBindTexture(GL_TEXTURE_2D, this->id);
  }

  void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}
