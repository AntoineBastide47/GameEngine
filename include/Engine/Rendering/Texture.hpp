//
// Texture2D.h
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef TEXTURE_H
#define TEXTURE_H
#include <string>

namespace Engine {
  class ResourceManager;
}
namespace Engine2D {
  class ParticleSystem2D;
  namespace Rendering {
    class Renderer2D;
  }
}

namespace Engine::Rendering {
  /// A class representing a 2D texture in OpenGL, handling creation, configuration, and binding of textures.
  class Texture {
    friend class Engine2D::Rendering::Renderer2D;
    friend class Engine::ResourceManager;
    friend class Engine2D::ParticleSystem2D;
    public:
      explicit Texture(const std::string &path);

      /// @returns the width of this texture
      inline int GetWidth() const {
        return width;
      }

      /// @returns the height of this texture
      inline int GetHeight() const {
        return height;
      }

      /// @returns the id of this texture
      inline unsigned int GetId() const {
        return id;
      }
    private:
      /// The OpenGL id of this texture
      unsigned int id;
      /// The width of this texture in pixels.
      int width;
      /// The height of this texture in pixels.
      int height;
      /// Where the texture file is stored on disc
      std::string path;

      /**
       * Sets up the texture with specified width, height, and pixel data.
       * @param width The width of the texture in pixels.
       * @param height The height of the texture in pixels.
       * @param data The pixel data for the texture, typically loaded from an image file.
       * @param internalFormat
       * @param dataFormat
       * @param blended Whether to blend the pixels or not
       */
      void generate(int width, int height, const unsigned char *data, int internalFormat, int dataFormat, bool blended = false);
      /// Makes this texture active so that subsequent rendering operations use it.
      void bind(int offset = 0) const;
      /// Unloads the texture data and prepares the object for reuse.
      void clear();
      /// Unbinds any texture bound to the GL_TEXTURE_2D target.
      static void unbind();
  };
}

#endif //TEXTURE_H
