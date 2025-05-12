//
// SpriteRenderer.h
// Author: Antoine Bastide
// Date: 10/11/2024
//

#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include <unordered_set>

#include "Engine2D/ParticleSystem/ParticleSystem2D.hpp"

namespace Engine2D {
  class Game2D;
  class Entity2D;
  class ParticleSystem2D;
}
namespace Engine {
  class ResourceManager;
  namespace Rendering {
    class Shader;
    class ShaderPreProcessor;
  }
}

using Engine::Rendering::Shader;

namespace Engine2D::Rendering {
  class SpriteRenderer;
  class Renderable2D;

  using Flush = std::tuple<uint32_t, uint32_t, uint32_t, uint32_t, uint32_t>;

  /// Class responsible for rendering all the textures/sprites in the game
  class Renderer2D {
    friend class Engine2D::Game2D;
    friend class Engine2D::Entity2D;
    friend class Engine2D::ParticleSystem2D;
    friend class Engine::ResourceManager;
    friend class Engine::Rendering::ShaderPreProcessor;
    public:
      static float PackTwoFloats(float a, float b);
    private:
      /// The vertices that will be sent to the VAO
      static const float vertices[];

      /// All the sprite renderers in the scene that are opaque
      static std::vector<std::shared_ptr<SpriteRenderer>> opaqueRenderers;
      /// All the sprite renderers in the scene that are transparent
      static std::vector<std::shared_ptr<SpriteRenderer>> transparentRenderers;
      /// All the static sprite renderers in the scene that are opaque
      static std::vector<std::shared_ptr<SpriteRenderer>> staticOpaqueRenderers;
      /// All the static sprite renderers in the scene that are transparent
      static std::vector<std::shared_ptr<SpriteRenderer>> staticTransparentRenderers;

      /// The renderers that are pending to be added but do not have a sprite or a texture
      static std::vector<std::shared_ptr<SpriteRenderer>> invalidRenderers;
      /// All the sprite renderers pending to be added
      static std::vector<std::shared_ptr<SpriteRenderer>> renderersToAdd;
      /// All the sprite renderers pending to be removed
      static std::unordered_set<std::shared_ptr<SpriteRenderer>> renderersToRemove;

      /// Stores the vertex array configuration for the quad.
      static uint quadVAO;
      /// Vertex buffer object used for vertex data
      static uint vertexVBO;
      /// Vertex buffer object used for batching
      static uint batchVBO;
      /// Vertex buffer object used for batching static opaque renderers
      static uint staticOpaqueBatchVBO;
      /// Vertex buffer object used for batching static transparent renderers
      static uint staticTransparentBatchVBO;
      /// The id of the previously drawn texture
      static uint lastBoundTexture;

      /// The current number of instances to render
      static uint instanceCount;
      /// The last shader that was used
      static uint lastShaderID;
      /// The number of static opaque sprites during the previous pass
      static size_t lastOpaqueStaticCount;
      /// The number of static transparent sprites during the previous pass
      static size_t lastTransparentStaticCount;

      /// Data linked to all the static renderers in the scene for opaque sprites
      static std::vector<float> staticOpaqueBatchData;
      /// Data linked to all the static renderers in the scene for transparent sprites
      static std::vector<float> staticTransparentBatchData;
      /// Tells the renderer how to use the static batch data for opaque sprites
      static std::vector<Flush> staticOpaqueFlushList;
      /// Tells the renderer how to use the static batch data for transparent sprites
      static std::vector<Flush> staticTransparentFlushList;
      /// Tells the renderer how to use the static batch data for opaque sprites
      static std::vector<Flush> opaqueFlushList;
      /// Tells the renderer how to use the static batch data for transparent sprites
      static std::vector<Flush> transparentFlushList;

      /// The maximum number of texture that can be active at once on the GPU
      static int MAX_TEXTURES;
      /// Maps a texture id to it's index in the gpu texture array
      static std::unordered_map<uint, uint> textureIdToIndexMap;
      /// Whether the renderables need to be sorted using the render order
      static bool zSort;

      Renderer2D() = default;
      ~Renderer2D();

      /// @return True if the renderer can be rendered, false if not
      static bool cannotBeRendered(const std::shared_ptr<Renderable2D> &r);
      /// Returns true if a needs to be before b, False if not
      static bool sortRenderers(const std::shared_ptr<Renderable2D> &a, const std::shared_ptr<Renderable2D> &b);
      /// Extracts all the data needed to render the sprite renderer
      static void extractRendererData(const std::shared_ptr<SpriteRenderer> &renderer, float *data);
      /// Maps the given id to an index to be used in the shader
      static void mapTextureIdToIndex(const uint &textureId);

      /// Builds and renders the given batch
      static void buildAndRenderStaticBatch(
        std::vector<std::shared_ptr<SpriteRenderer>> &renderers, std::vector<float> &batchData,
        std::vector<Flush> &flushList, bool rebuild, uint VBO, uint framebuffer = 0
      );
      /// Updates the data linked to the stored static sprites
      static void updateBatch(
        const std::vector<std::shared_ptr<SpriteRenderer>> &renderers, std::vector<float> &staticBatchData
      );
      /// Builds and renders the given batch
      static void buildAndRenderBatch(
        std::vector<std::shared_ptr<Renderable2D>> &renderers, std::vector<Flush> &flushList, uint particleCount,
        uint framebuffer = 0
      );
      /// Changes the blend mode of the flush list
      static void setBlendMode(
        bool opaque, bool isParticle, ParticleSystem2D::BlendMode blendMode = ParticleSystem2D::BlendMode::Alpha
      );
      /// Sends all the data to the GPU
      static void flush(uint VBO, const float *data, int drawMode, uint32_t count, uint32_t framebuffer = 0);

      /// Initializes the vertex data and buffers for rendering a quad.
      static void initRenderData();
      /// Partitions the sprites for easier rendering
      static void updateAndSplitRenderList();
      /// Renders all the opaque sprites
      static void opaquePass(uint framebuffer = 0);
      /// Renders all the transparent sprites
      static void transparentPass(uint framebuffer = 0);

      /// Renders all sprites
      static void render(uint framebuffer = 0);

      /// Adds a new sprite renderer to the list of ones to be rendered
      static void addRenderer(const std::shared_ptr<SpriteRenderer> &renderer);
      /// Removes a sprite renderer from the list of ones to be rendered
      static void removeRenderer(const std::shared_ptr<SpriteRenderer> &renderer);
  };
}

#endif // SPRITE_RENDERER_H
