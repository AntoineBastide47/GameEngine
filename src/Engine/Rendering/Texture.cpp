//
// Texture2D.cpp
// Author: Antoine Bastide
// Date: 10/11/2024
//

#include "Engine/Rendering/Texture.hpp"
#include "Engine/RenderingHeaders.hpp"

namespace Engine::Rendering {
  Texture::Texture()
    : id(0), width(0), height(0), internalFormat(GL_RGB), imageFormat(GL_RGB), wrapS(GL_CLAMP_TO_EDGE),
      wrapT(GL_CLAMP_TO_EDGE), filterMin(GL_NEAREST), filterMax(GL_NEAREST) {}

  void Texture::generate(const int width, const int height, const unsigned char *data) {
    this->width = width;
    this->height = height;

    // create Texture
    glGenTextures(1, &this->id);
    glBindTexture(GL_TEXTURE_2D, this->id);
    glTexImage2D(GL_TEXTURE_2D, 0, this->internalFormat, width, height, 0, this->imageFormat, GL_UNSIGNED_BYTE, data);

    // set Texture wrap and filter modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->wrapS);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->wrapT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->filterMin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->filterMax);

    // unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  void Texture::clear() {
    if (this->id > 0) {
      glDeleteTextures(1, &this->id);
      id = 0;
    }
  }

  void Texture::bind() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->id);
  }

  void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}
