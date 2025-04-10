//
// SpriteRenderer.h
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#define STRIDE (27 * sizeof(float))
#define MAX_BATCH_SIZE (128 * 1024 * 1024)
#define MAX_BATCH_SIZE_WITH_STRIDE (MAX_BATCH_SIZE / STRIDE)
#define MAX_INSTANCE_COUNT 10000

namespace Engine2D::Rendering {
  class SpriteRenderer;
}
namespace Engine2D {
  class Game2D;
  class Entity2D;
}
namespace Engine::Rendering {
  class Shader;
}

using Engine::Rendering::Shader;

namespace Engine2D::Rendering {
  /// Class responsible for rendering all the textures/sprites in the game
  class Renderer2D {
    friend class Engine2D::Game2D;
    friend class Engine2D::Entity2D;

    /// The vertices that will be sent to the VAO
    static const float vertices[24];

    /// All the sprite renderers in the scene
    static std::vector<std::shared_ptr<SpriteRenderer>> renderers;
    /// All the sprite renderers pending to be added
    static std::vector<std::shared_ptr<SpriteRenderer>> renderersToAdd;
    /// All the sprite renderers pending to be removed
    static std::vector<std::shared_ptr<SpriteRenderer>> renderersToRemove;

    /// Stores the vertex array configuration for the quad.
    static uint quadVAO;
    /// Vertex buffer object used for batching
    static uint batchVBO;
    /// The id of the previously drawn texture
    static uint lastBoundTexture;

    /// The current number of instances to render
    static uint instanceCount;
    /// The last shader that was used
    static uint lastShaderID;
    /// The last texture id that was used
    static uint lastTextureID;
    /// Data linked to all the renderers in the scene
    static std::vector<float> batchData;

    Renderer2D();
    ~Renderer2D();

    /// Initializes the vertex data and buffers for rendering a quad.
    static void initRenderData();
    /// Renders all the given entities
    static void render();
    /// Sends all the data to the GPU
    static void flush();
    /// Adds a new sprite renderer to the list of ones to be rendered
    static void addRenderer(const std::shared_ptr<SpriteRenderer> &renderer);
    /// Removes a sprite renderer from the list of ones to be rendered
    static void removeRenderer(const std::shared_ptr<SpriteRenderer> &renderer);
  };
}

#endif // SPRITE_RENDERER_H
