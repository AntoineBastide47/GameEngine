//
// Texture2D.h
// Texture2D: 
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef TEXTURE2D_H
#define TEXTURE2D_H

namespace Engine2D::Rendering {
  class Texture2D {
    public:
      // holds the ID of the texture object, used for all texture operations to reference to this particular texture
      unsigned int id;
      // texture image dimensions
      int width, height; // width and height of loaded image in pixels
      // texture Format
      int internalFormat; // format of texture object
      int imageFormat;    // format of loaded image
      // texture configuration
      int wrapS;     // wrapping mode on S axis
      int wrapT;     // wrapping mode on T axis
      int filterMin; // filtering mode if texture pixels < screen pixels
      int filterMax; // filtering mode if texture pixels > screen pixels
      // constructor (sets default texture modes)
      Texture2D();
      // generates texture from image data
      void Generate(int width, int height, const unsigned char *data);
      // binds the texture as the current active GL_TEXTURE_2D texture object
      void Bind() const;

      void Clear();

      static void Unbind();
  };
}

#endif //TEXTURE2D_H
