//
// SpriteRenderer.h
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef SPRITERENDERER_H
#define SPRITERENDERER_H

#include <map>

#include "Shader.h"
#include "2D/Entity2D.h"

namespace Engine2D {
  class Game2D;
}

namespace Engine2D::Rendering {
  /// Class responsible for rendering all the textures/sprites in the game
  class SpriteRenderer {
    friend class Engine2D::Game2D;
    /// Shader that controls the rendering pipeline for the sprite, including transformations and textures.
    static Shader *shader;
    /// Stores the vertex configuration for the quad, making rendering efficient.
    static unsigned int quadVAO;
    /// The id of the previously drawn texture
    static unsigned int lastBoundTexture;

    SpriteRenderer() = default;
    ~SpriteRenderer();

    /**
     * Initializes the vertex data and buffers for rendering a quad.
     * This setup is performed once in the constructor and is used throughout the object's lifetime.
     */
    static void initRenderData();
    /// Renders all the given entities
    static void drawSprites(const std::map<int, std::vector<Entity2D *> > &entities);
  };
}

#endif // SPRITERENDERER_H
