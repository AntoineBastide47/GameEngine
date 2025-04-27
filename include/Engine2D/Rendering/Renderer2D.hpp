//
// SpriteRenderer.h
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#define STRIDE (20)
#define STRIDE_SIZE (STRIDE * sizeof(float))
#define MAX_BATCH_SIZE (1024 * 1024)
#define MAX_BATCH_SIZE_WITH_STRIDE (MAX_BATCH_SIZE / STRIDE_SIZE)
#define MAX_INSTANCE_COUNT 10000
#include <unordered_set>

namespace Engine2D {
  class Game2D;
  class Entity2D;
}
namespace Engine::Rendering {
  class Shader;
}

using Engine::Rendering::Shader;

namespace Engine2D::Rendering {
  class SpriteRenderer;

  /// Class responsible for rendering all the textures/sprites in the game
  class Renderer2D {
    friend class Engine2D::Game2D;
    friend class Engine2D::Entity2D;

    /// The vertices that will be sent to the VAO
    static const float vertices[];

    /// All the sprite renderers in the scene
    static std::vector<std::shared_ptr<SpriteRenderer>> renderers;
    /// All the static sprite renderers in the scene
    static std::vector<std::shared_ptr<SpriteRenderer>> staticRenderers;

    /// All the sprite renderers pending to be added
    static std::vector<std::shared_ptr<SpriteRenderer>> renderersToAdd;
    /// All the sprite renderers pending to be removed
    static std::unordered_set<std::shared_ptr<SpriteRenderer>> renderersToRemove;

    /// Stores the vertex array configuration for the quad.
    static uint quadVAO;
    /// Vertex buffer object used for batching
    static uint batchVBO;
    /// Vertex buffer object used for batching
    static uint staticBatchVBO;
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
    /// Data linked to all the renderers in the scene
    static std::vector<float> staticBatchData;
    /// Tells the renderer how to use the static batch data
    static std::vector<std::tuple<uint32_t, uint32_t, uint32_t, uint32_t>> staticFlushList;

    Renderer2D();
    ~Renderer2D();

    /// Initializes the vertex data and buffers for rendering a quad.
    static void initRenderData();
    /// @return True if the renderer can be rendered, false if not
    static bool cannotBeRendered(const std::shared_ptr<SpriteRenderer> &r);
    /// Returns true if a needs to be before b, False if not
    static bool sortRenderers(const std::shared_ptr<SpriteRenderer> &a, const std::shared_ptr<SpriteRenderer> &b);
    /// Extracts all the data needed to render the sprite renderer
    static void extractRendererData(const std::shared_ptr<SpriteRenderer> &renderer, float *data);

    /// Updates the data linked to the stored static sprites
    static void updateStaticBatch();
    /// Builds and renders the dynamic batch
    static void buildAndRenderStaticBatch(bool rebuild);
    /// Builds and renders the dynamic batch
    static void buildAndRenderDynamicBatch();
    /// Renders all the given entities
    static void render();
    /// Sends all the data to the GPU
    static void flush(uint VBO, const float *data, int drawMode, uint32_t count, uint32_t framebuffer = 0);

    /// Adds a new sprite renderer to the list of ones to be rendered
    static void addRenderer(const std::shared_ptr<SpriteRenderer> &renderer);
    /// Removes a sprite renderer from the list of ones to be rendered
    static void removeRenderer(const std::shared_ptr<SpriteRenderer> &renderer);
  };
}

#endif // SPRITE_RENDERER_H
