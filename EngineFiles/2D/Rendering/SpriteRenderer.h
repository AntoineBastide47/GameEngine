//
// SpriteRenderer.h
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef SPRITERENDERER_H
#define SPRITERENDERER_H

#include "Shader.h"
#include "2D/Entity2D.h"

namespace Engine2D {
  class Game2D;
}

namespace Engine2D::Rendering {
  /**
   * A class responsible for rendering 2D sprites using OpenGL.
   * This class uses a shader to draw textured quads representing sprites and is optimized for rendering 2D game objects.
   */
  class SpriteRenderer {
    friend class Engine2D::Game2D;
    public:
      /**
       * Renders a sprite based on an entity's transform and specified color.
       * Uses the shader to render a textured quad for the given entity, allowing for color tinting.
       *
       * @param entity Pointer to an Entity2D object that provides position, rotation, and scaling information.
       */
      static void DrawSprite(const Entity2D *entity);
    private:
      /** Shader that controls the rendering pipeline for the sprite, including transformations and textures. */
      static Shader *shader;
      /** Stores the vertex configuration for the quad, making rendering efficient. */
      static unsigned int quadVAO;
      /** The id of the previously drawn texture */
      static unsigned int lastBoundTexture;

      /** */
      SpriteRenderer() = default;
      /** Cleans up any allocated OpenGL resources. */
      ~SpriteRenderer();

      /**
       * Initializes the vertex data and buffers for rendering a quad.
       * This setup is performed once in the constructor and is used throughout the object's lifetime.
       */
      static void initRenderData();
  };
}

#endif // SPRITERENDERER_H
