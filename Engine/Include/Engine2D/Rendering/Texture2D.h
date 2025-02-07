//
// Texture2D.h
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef TEXTURE2D_H
#define TEXTURE2D_H

namespace Engine2D {
  class ResourceManager;
  class ParticleSystem2D;
}

namespace Engine2D::Rendering {
  /// A class representing a 2D texture in OpenGL, handling creation, configuration, and binding of textures.
  class Texture2D {
    friend class SpriteRenderer;
    friend class Engine2D::ResourceManager;
    friend class Engine2D::ParticleSystem2D;
    public:
      /// The OpenGL id of this texture
      uint id;
      ///  The width of this texture in pixels.
      int width;
      ///  The height of this texture in pixels.
      int height;
      /// Specifies the format in which OpenGL stores the texture (e.g., GL_RGB, GL_RGBA).
      int internalFormat;
      ///  Specifies the format of the image data being passed in (e.g., GL_RGB, GL_RGBA).
      int imageFormat;
      /// Defines how the texture should behave if the coordinates exceed the texture bounds horizontally (e.g., GL_REPEAT, GL_CLAMP_TO_EDGE).
      int wrapS;
      /// Defines how the texture should behave if the coordinates exceed the texture bounds vertically (e.g., GL_REPEAT, GL_CLAMP_TO_EDGE).
      int wrapT;
      /// Determines how the texture is sampled when minified (e.g., GL_LINEAR, GL_NEAREST).
      int filterMin;
      /// Determines how the texture is sampled when magnified (e.g., GL_LINEAR, GL_NEAREST).
      int filterMax;

      Texture2D();

    private:
      /**
       * Sets up the texture with specified width, height, and pixel data.
       * @param width The width of the texture in pixels.
       * @param height The height of the texture in pixels.
       * @param data The pixel data for the texture, typically loaded from an image file.
       */
      void generate(int width, int height, const unsigned char *data);
      /// Makes this texture active so that subsequent rendering operations use it.
      void bind() const;
      /// Unloads the texture data and prepares the object for reuse.
      void clear();
      /// Unbinds any texture bound to the GL_TEXTURE_2D target.
      static void unbind();
  };
}

#endif //TEXTURE2D_H
