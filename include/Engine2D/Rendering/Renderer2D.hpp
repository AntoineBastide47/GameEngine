//
// Renderer2D.h
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
    friend class SpriteRenderer;
    friend class Engine2D::Game2D;
    friend class Engine2D::ParticleSystem2D;
    friend class Engine::ResourceManager;
    friend class Engine::Rendering::ShaderPreProcessor;
    public:
      static float PackTwoFloats(float a, float b);
    private:
      /// The vertices that will be sent to the VAO
      static constexpr float vertices[] = {
        // pos      // tex
        0.0f, 0.0f, 0.0f, 1.0f, // Bottom-left
        1.0f, 0.0f, 1.0f, 1.0f, // Bottom-right
        0.0f, 1.0f, 0.0f, 0.0f, // Top-left
        1.0f, 1.0f, 1.0f, 0.0f // Top-right
      };

      /// All the sprite renderers in the scene that are opaque
      inline static std::vector<SpriteRenderer *> opaqueRenderers;
      /// All the sprite renderers in the scene that are transparent
      inline static std::vector<SpriteRenderer *> transparentRenderers;
      /// All the static sprite renderers in the scene that are opaque
      inline static std::vector<SpriteRenderer *> staticOpaqueRenderers;
      /// All the static sprite renderers in the scene that are transparent
      inline static std::vector<SpriteRenderer *> staticTransparentRenderers;

      /// The renderers that are pending to be added but do not have a sprite or a texture
      inline static std::vector<SpriteRenderer *> invalidRenderers;
      /// All the sprite renderers pending to be added
      inline static std::vector<SpriteRenderer *> renderersToAdd;
      /// All the sprite renderers pending to be removed
      inline static std::unordered_set<SpriteRenderer *> renderersToRemove;

      /// Stores the vertex array configuration for the quad.
      inline static uint quadVAO = 0;
      /// Vertex buffer object used for vertex data
      inline static uint vertexVBO = 0;
      /// Vertex buffer object used for batching
      inline static uint batchVBO = 0;
      /// Vertex buffer object used for batching static opaque renderers
      inline static uint staticOpaqueBatchVBO = 0;
      /// Vertex buffer object used for batching static transparent renderers
      inline static uint staticTransparentBatchVBO = 0;
      /// The id of the previously drawn texture
      inline static uint lastBoundTexture = 0;

      /// The current number of instances to render
      inline static uint instanceCount = 0;
      /// The last shader that was used
      inline static uint lastShaderID = 0;
      /// The number of static opaque sprites during the previous pass
      inline static size_t lastOpaqueStaticCount = 0;
      /// The number of static transparent sprites during the previous pass
      inline static size_t lastTransparentStaticCount = 0;

      /// Data linked to all the static renderers in the scene for opaque sprites
      inline static std::vector<float> staticOpaqueBatchData;
      /// Data linked to all the static renderers in the scene for transparent sprites
      inline static std::vector<float> staticTransparentBatchData;
      /// Tells the renderer how to use the static batch data for opaque sprites
      inline static std::vector<Flush> staticOpaqueFlushList;
      /// Tells the renderer how to use the static batch data for transparent sprites
      inline static std::vector<Flush> staticTransparentFlushList;
      /// Tells the renderer how to use the static batch data for opaque sprites
      inline static std::vector<Flush> opaqueFlushList;
      /// Tells the renderer how to use the static batch data for transparent sprites
      inline static std::vector<Flush> transparentFlushList;

      /// The maximum number of texture that can be active at once on the GPU
      inline static int MAX_TEXTURES = 0;
      /// Maps a texture id to it's index in the gpu texture array
      inline static std::unordered_map<uint, uint> textureIdToIndexMap;
      /// Whether the renderables need to be sorted using the render order
      inline static bool zSort = false;

      Renderer2D() = default;
      ~Renderer2D();

      /// @return True if the renderer can be rendered, false if not
      static bool cannotBeRendered(const Renderable2D *r);
      /// Returns true if a needs to be before b, False if not
      static bool sortRenderers(const Renderable2D *a, const Renderable2D *b);
      /// Extracts all the data needed to render the sprite renderer
      static void extractRendererData(const SpriteRenderer *renderer, float *data);
      /// Maps the given id to an index to be used in the shader
      static void mapTextureIdToIndex(const uint &textureId);

      /// Builds and renders the given batch
      static void buildAndRenderStaticBatch(
        std::vector<SpriteRenderer *> &renderers, std::vector<float> &batchData,
        std::vector<Flush> &flushList, bool rebuild, uint VBO, uint framebuffer = 0
      );
      /// Updates the data linked to the stored static sprites
      static void updateBatch(
        const std::vector<SpriteRenderer *> &renderers, std::vector<float> &staticBatchData
      );
      /// Builds and renders the given batch
      static void buildAndRenderBatch(
        std::vector<Renderable2D *> &renderers, std::vector<Flush> &flushList, uint particleCount,
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
      static void addRenderer(SpriteRenderer *renderer);
      /// Removes a sprite renderer from the list of ones to be rendered
      static void removeRenderer(SpriteRenderer *renderer);
  };
}

#endif // SPRITE_RENDERER_H
