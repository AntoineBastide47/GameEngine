//
// Texture2D.h
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef TEXTURE2D_H
#define TEXTURE2D_H

namespace Engine2D::Rendering {
  /** A class representing a 2D texture in OpenGL, handling creation, configuration, and binding of textures. */
  class Texture2D {
    public:
      /**
       * The OpenGL ID for this texture object.
       * Used for all texture operations to reference this particular texture.
       */
      unsigned int id;
      /**  The width of this texture in pixels. */
      int width;
      /**  The height of this texture in pixels. */
      int height;
      /** Specifies the format in which OpenGL stores the texture (e.g., GL_RGB, GL_RGBA). */
      int internalFormat;
      /**  Specifies the format of the image data being passed in (e.g., GL_RGB, GL_RGBA). */
      int imageFormat;
      /** Defines how the texture should behave if the coordinates exceed the texture bounds horizontally (e.g., GL_REPEAT, GL_CLAMP_TO_EDGE). */
      int wrapS;
      /** Defines how the texture should behave if the coordinates exceed the texture bounds vertically (e.g., GL_REPEAT, GL_CLAMP_TO_EDGE). */
      int wrapT;
      /** Determines how the texture is sampled when minified (e.g., GL_LINEAR, GL_NEAREST). */
      int filterMin;
      /** Determines how the texture is sampled when magnified (e.g., GL_LINEAR, GL_NEAREST). */
      int filterMax;

      /** Default Constructor */
      Texture2D();

      /**
       * Sets up the texture with specified width, height, and pixel data.
       *
       * @param width The width of the texture in pixels.
       * @param height The height of the texture in pixels.
       * @param data The pixel data for the texture, typically loaded from an image file.
       */
      void Generate(int width, int height, const unsigned char *data);

      /** Makes this texture active so that subsequent rendering operations use it. */
      void Bind() const;

      /** Unloads the texture data and prepares the object for reuse. */
      void Clear();

      /** Unbinds any texture bound to the GL_TEXTURE_2D target. */
      static void Unbind();
  };
}

#endif //TEXTURE2D_H
