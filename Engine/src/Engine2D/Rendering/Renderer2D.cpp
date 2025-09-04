//
// Renderer2D.cpp
// Author: Antoine Bastide
// Date: 10/11/2024
//

#include <numeric>
#include <vector>

#include "Engine2D/Rendering/Renderer2D.hpp"
#include "Engine/ResourceManager.hpp"
#include "Engine/Macros/Assert.hpp"
#include "Engine/Macros/Profiling.hpp"
#include "Engine2D/Entity2D.hpp"
#include "Engine/Rendering/Shader.hpp"
#include "Engine2D/Rendering/Sprite.hpp"
#include "Engine2D/Rendering/SpriteRenderer.hpp"
#include "Engine/Rendering/Texture.hpp"
#include "Engine2D/Game2D.hpp"
#include "Engine2D/SceneManagement/SceneManager.hpp"
#include "Engine2D/ParticleSystem/ParticleSystemRegistry2D.hpp"

#define STRIDE (16)
#define STRIDE_SIZE (STRIDE * sizeof(float))
#define MAX_BATCH_SIZE (1024 * 1024)
#define MAX_BATCH_SIZE_WITH_STRIDE (MAX_BATCH_SIZE / STRIDE_SIZE)
#define MAX_INSTANCE_COUNT 10000

namespace Engine2D::Rendering {
  Renderer2D::~Renderer2D() {
    if (quadVAO > 0) {
      glDeleteVertexArrays(1, &quadVAO);
      quadVAO = 0;
    }
    if (vertexVBO > 0) {
      glDeleteBuffers(1, &vertexVBO);
      vertexVBO = 0;
    }
    if (batchVBO > 0) {
      glDeleteBuffers(1, &batchVBO);
      batchVBO = 0;
    }
    if (staticOpaqueBatchVBO > 0) {
      glDeleteBuffers(1, &staticOpaqueBatchVBO);
      staticOpaqueBatchVBO = 0;
    }
    if (staticTransparentBatchVBO > 0) {
      glDeleteBuffers(1, &staticTransparentBatchVBO);
      staticTransparentBatchVBO = 0;
    }
  }

  float Renderer2D::PackTwoFloats(const float a, const float b) {
    // Map from [-1, 1] to [0, 65535]
    uint packedA = static_cast<uint>((a + 1.0f) * 32767.5f);
    uint packedB = static_cast<uint>((b + 1.0f) * 32767.5f);

    // Clamp to ensure we stay within 16-bit range
    packedA = glm::clamp(packedA, 0u, 65535u);
    packedB = glm::clamp(packedB, 0u, 65535u);

    return glm::uintBitsToFloat(packedA << 16 | packedB);
  }

  bool Renderer2D::cannotBeRendered(const Renderable2D *r) {
    return !r->Entity()->IsActive() || !r->IsActive() || !r->Transform()->IsVisible() || !r->shader ||
           !r->sprite || !r->sprite->texture;
  }

  bool Renderer2D::sortRenderers(const Renderable2D *a, const Renderable2D *b) const {
    if (const bool aValid = cannotBeRendered(a); aValid != cannotBeRendered(b))
      return !aValid;

    if (zSort && a->renderOrder != b->renderOrder)
      return a->renderOrder < b->renderOrder;
    if (a->shader->id != b->shader->id)
      return a->shader->id < b->shader->id;
    return a->sprite->texture->id < b->sprite->texture->id;
  }

  void Renderer2D::extractRendererData(const SpriteRenderer *renderer, float *data) const {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerFunction);

    if (cannotBeRendered(renderer))
      return;

    // Get the model matrix and other data
    const SpriteRenderer &r = *renderer;
    const auto sprite = r.sprite;
    const auto color = r.color;
    const float invPPU = 1.0f / sprite->pixelsPerUnit;
    const auto rect = sprite->rect;

    // Position and scale
    *data++ = r.Transform()->WorldPosition().x;
    *data++ = r.Transform()->WorldPosition().y;
    *data++ = r.Transform()->WorldScale().x * invPPU;
    *data++ = r.Transform()->WorldScale().y * invPPU;

    // Color
    *data++ = std::clamp(color.x, 0.0f, 1.0f);
    *data++ = std::clamp(color.y, 0.0f, 1.0f);
    *data++ = std::clamp(color.z, 0.0f, 1.0f);
    *data++ = std::clamp(color.w, 0.0f, 1.0f);

    // Rect
    *data++ = std::clamp(rect.x, 0.0f, 1.0f);
    *data++ = std::clamp(rect.y, 0.0f, 1.0f);
    *data++ = std::clamp(rect.z, 0.0f, 1.0f);
    *data++ = std::clamp(rect.w, 0.0f, 1.0f);

    // Pivot, rotation, render order, texture index and flip
    *data++ = PackTwoFloats(
      std::clamp(r.sprite->pivot.x, -1.0f, 1.0f),
      std::clamp(r.sprite->pivot.y, -1.0f, 1.0f)
    );
    *data++ = r.Transform()->WorldRotation();
    *data++ = r.renderOrder << 16 | textureIdToIndexMap.at(r.sprite->texture->id);
    *data = PackTwoFloats(r.Flip().x ? -1.0f : 1.0, r.Flip().y ? -1.0f : 1.0);
  }

  void Renderer2D::mapTextureIdToIndex(const uint &textureId) {
    // Do nothing for invalid texture id's
    if (textureId == 0) {
      return;
    }

    // Do nothing if the id is already mapped
    if (textureIdToIndexMap.contains(textureId)) {
      Engine::ResourceManager::GetTexture2DById(textureId)->bind(textureIdToIndexMap.at(textureId));
      return;
    }

    // Reset the mapping
    if (textureIdToIndexMap.size() >= MAX_TEXTURES)
      textureIdToIndexMap.clear();

    // Create the mapping
    const int texIndex = static_cast<int>(textureIdToIndexMap.size());
    textureIdToIndexMap.emplace(textureId, texIndex);
    Engine::ResourceManager::GetTexture2DById(textureId)->bind(texIndex);
  }

  void Renderer2D::buildAndRenderStaticBatch(
    std::vector<SpriteRenderer *> &renderers, std::vector<float> &batchData,
    std::vector<Flush> &flushList, const bool rebuild, const uint VBO, const uint framebuffer
  ) {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerSystem);

    if (rebuild) {
      std::ranges::sort(
        renderers, [this](const Renderable2D *a, const Renderable2D *b) {
          return sortRenderers(a, b);
        }
      );

      // Only extract data for valid sprite renderers
      uint32_t start = 0, count = 0, currentShaderID = 0, currentTextureID = 0, index = 0;

      flushList.clear();
      batchData.clear();
      batchData.reserve(renderers.size() * STRIDE);
      batchData.resize(renderers.size() * STRIDE);

      for (const auto &renderer: renderers) {
        // Check for change in shader or texture ID
        const uint32_t shaderID = renderer->shader->id;
        if (const uint32_t textureID = renderer->sprite->texture->id;
          shaderID != currentShaderID || textureID != currentTextureID) {
          // Save the previous flush range if any
          if (count > 0) {
            flushList.emplace_back(currentShaderID, currentTextureID, start, count, 10000);
            start += count;
            count = 0;
          }

          currentShaderID = shaderID;
          currentTextureID = textureID;
          mapTextureIdToIndex(textureID);
        }

        extractRendererData(renderer, &batchData[index * STRIDE]);
        count++;
        index++;
      }
      if (count > 0)
        flushList.emplace_back(currentShaderID, currentTextureID, start, count, 10000);
    } else
      updateBatch(renderers, batchData);

    for (auto &[shaderID, textureID, start, count, blendMode]: flushList) {
      if (shaderID == 0 || textureID == 0)
        continue;

      if (shaderID != lastShaderID) {
        lastShaderID = shaderID;
        Engine::ResourceManager::GetShaderById(shaderID)->use();
      }

      mapTextureIdToIndex(textureID);
      setBlendMode(blendMode >> 5 == 1, false, ParticleSystem2D::Alpha);
      flush(VBO, &batchData[start * STRIDE], GL_STATIC_DRAW, count, framebuffer);
    }
  }

  void Renderer2D::updateBatch(
    const std::vector<SpriteRenderer *> &renderers, std::vector<float> &staticBatchData
  ) const {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerSubSystem);

    for (size_t i = 0; i < renderers.size(); i++) {
      auto &renderer = renderers[i];
      const size_t offset = i * STRIDE;
      if (cannotBeRendered(renderer)) {
        auto data = &staticBatchData[offset];
        data += 2;
        *data++ = 0.0f;
        *data = 0.0f;
      } else if (renderer->dirty) {
        extractRendererData(renderer, &staticBatchData[offset]);
        renderer->dirty = false;
      }
    }
  }

  void Renderer2D::buildAndRenderBatch(
    std::vector<Renderable2D *> &renderers, std::vector<Flush> &flushList, const uint particleCount, const bool resort,
    const uint framebuffer
  ) {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerSystem);

    if (resort)
      std::ranges::sort(
        renderers, [this](const Renderable2D *a, const Renderable2D *b) {
          return sortRenderers(a, b);
        }
      );

    flushList.clear();

    uint32_t start = 0, count = 0, currentShaderID = 0, currentTextureID = 0, index = 0;

    // Only extract data for valid sprite renderers
    const auto it = std::ranges::find_if(renderers, cannotBeRendered);
    const auto validRange = std::ranges::subrange(renderers.begin(), it);

    // Bind the target frame buffer
    if (framebuffer > 0)
      glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glBindBuffer(GL_ARRAY_BUFFER, batchVBO);
    glBufferData(GL_ARRAY_BUFFER, (validRange.size() + particleCount) * STRIDE_SIZE, nullptr, GL_STREAM_DRAW);

    const auto gpuPtr = static_cast<float *>(glMapBufferRange(
      GL_ARRAY_BUFFER, 0, (validRange.size() + particleCount) * STRIDE_SIZE,
      GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT
    ));

    if (!gpuPtr)
      return;

    for (const auto &renderer: validRange) {
      // Send the previous sprites to the flush list
      const uint32_t shaderID = renderer->shader->id;
      const uint32_t textureID = renderer->sprite->texture->id;
      if (currentShaderID != shaderID || currentTextureID != textureID) {
        if (count > 0) {
          flushList.emplace_back(currentShaderID, currentTextureID, start, count, !zSort << 5 | 0);
          start += count;
          count = 0;
        }

        currentShaderID = shaderID;
        currentTextureID = textureID;
      }

      mapTextureIdToIndex(textureID);

      if (renderer->renderType == Renderable2D::ParticleSystem) {
        const auto particleSystem = dynamic_cast<ParticleSystem2D *>(renderer);
        const int capacity = particleSystem->capacity;
        particleSystem->updateAndRender(textureIdToIndexMap.at(textureID), &gpuPtr[index * STRIDE]);

        // Send the particles to the flush list
        flushList.emplace_back(
          currentShaderID, currentTextureID, start, capacity,
          !zSort << 5 | 1 << 4 | static_cast<int>(particleSystem->blendMode)
        );
        start += capacity;
        index += capacity;
      } else {
        extractRendererData(static_cast<SpriteRenderer *>(renderer), &gpuPtr[index * STRIDE]);
        count++;
        index++;
      }
    }
    if (count > 0)
      flushList.emplace_back(currentShaderID, currentTextureID, start, count, !zSort << 5 | 0);

    glUnmapBuffer(GL_ARRAY_BUFFER);

    for (auto &[shaderID, textureID, start, count, blendMode]: flushList) {
      if (shaderID == 0 || textureID == 0)
        continue;

      if (shaderID != lastShaderID) {
        lastShaderID = shaderID;
        Engine::ResourceManager::GetShaderById(shaderID)->use();
      }

      mapTextureIdToIndex(textureID);
      const bool isParticle = (blendMode >> 4 & 1) == 1;
      const auto blend = static_cast<ParticleSystem2D::BlendMode>(blendMode & 7);
      setBlendMode(blendMode >> 5 == 1, isParticle, blend);
      flush(batchVBO, &gpuPtr[start * STRIDE], GL_DYNAMIC_DRAW, count, framebuffer);

      if (isParticle && blend == ParticleSystem2D::Subtractive)
        glBlendEquation(GL_FUNC_ADD);
    }

    // Unbind the target frame buffer
    if (framebuffer > 0)
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void Renderer2D::setBlendMode(const bool opaque, const bool isParticle, const ParticleSystem2D::BlendMode blendMode) {
    static GLint lastSrcFactor = -1;
    static GLint lastDstFactor = -1;
    static GLint lastBlendEq = -1;

    GLint srcFactor = GL_ZERO, dstFactor = GL_ZERO, blendEq = GL_FUNC_ADD;

    if (!isParticle) {
      if (opaque) {
        srcFactor = GL_ONE;
        dstFactor = GL_ZERO;
      } else {
        srcFactor = GL_SRC_ALPHA;
        dstFactor = GL_ONE_MINUS_SRC_ALPHA;
      }
    } else {
      switch (blendMode) {
        case ParticleSystem2D::BlendMode::Alpha:
          srcFactor = GL_SRC_ALPHA;
          dstFactor = GL_ONE_MINUS_SRC_ALPHA;
          break;
        case ParticleSystem2D::BlendMode::Additive:
          srcFactor = GL_ONE;
          dstFactor = GL_ONE;
          break;
        case ParticleSystem2D::BlendMode::Multiply:
          srcFactor = GL_DST_COLOR;
          dstFactor = GL_ZERO;
          break;
        case ParticleSystem2D::BlendMode::PremultipliedAlpha:
          srcFactor = GL_ONE;
          dstFactor = GL_ONE_MINUS_SRC_ALPHA;
          break;
        case ParticleSystem2D::BlendMode::Subtractive:
          srcFactor = GL_ONE;
          dstFactor = GL_ONE;
          blendEq = GL_FUNC_REVERSE_SUBTRACT;
          break;
        case ParticleSystem2D::BlendMode::SoftAdditive:
          srcFactor = GL_ONE_MINUS_DST_COLOR;
          dstFactor = GL_ONE;
          break;
        case ParticleSystem2D::BlendMode::Opaque:
          srcFactor = GL_ONE;
          dstFactor = GL_ZERO;
          break;
        default:
          srcFactor = GL_SRC_ALPHA;
          dstFactor = GL_ONE_MINUS_SRC_ALPHA;
          break;
      }
    }

    if (srcFactor != lastSrcFactor || dstFactor != lastDstFactor) {
      glBlendFunc(srcFactor, dstFactor);
      lastSrcFactor = srcFactor;
      lastDstFactor = dstFactor;
    }

    if (blendEq != lastBlendEq) {
      glBlendEquation(blendEq);
      lastBlendEq = blendEq;
    }
  }

  void Renderer2D::flush(
    const uint VBO, const float *data, const int drawMode, const uint32_t count, const uint32_t framebuffer
  ) {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerFunction);

    if (!count || !data)
      return;

    // Bind the target frame buffer
    if (framebuffer > 0)
      glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // Update the instance VBO with the new data.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, count * STRIDE_SIZE, data, drawMode);

    // Reset all the vertex attribute pointers
    constexpr GLsizei vec4Size = 4 * sizeof(float);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, nullptr);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, reinterpret_cast<void *>(vec4Size));
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, reinterpret_cast<void *>(2 * vec4Size));
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, reinterpret_cast<void *>(3 * vec4Size));

    // Draw all sprites in the batch in a single call.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, count);

    // Unbind the target frame buffer
    if (framebuffer > 0)
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  void Renderer2D::initRenderData() {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerFunction);

    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    // --- Set up the quad’s vertex buffer ---
    glGenBuffers(1, &vertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Attribute 0: position and texCoord
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), nullptr);

    // --- Set up the instance buffer ---
    glGenBuffers(1, &batchVBO);
    glBindBuffer(GL_ARRAY_BUFFER, batchVBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    // Setup instance attributes:
    constexpr GLsizei vec4Size = 4 * sizeof(float);
    // Attribute location 1: position and scale
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, nullptr);
    glVertexAttribDivisor(1, 1);

    // Attribute location 2: color
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, reinterpret_cast<void *>(vec4Size));
    glVertexAttribDivisor(2, 1);

    // Attribute location 3: rect
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, reinterpret_cast<void *>(2 * vec4Size));
    glVertexAttribDivisor(3, 1);

    // Attribute location 4: pivot, rotation and renderOrder
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, STRIDE_SIZE, reinterpret_cast<void *>(3 * vec4Size));
    glVertexAttribDivisor(4, 1);

    // --- Set up the static instance buffers ---
    glGenBuffers(1, &staticOpaqueBatchVBO);
    glBindBuffer(GL_ARRAY_BUFFER, staticOpaqueBatchVBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);
    glGenBuffers(1, &staticTransparentBatchVBO);
    glBindBuffer(GL_ARRAY_BUFFER, staticTransparentBatchVBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_STATIC_DRAW);

    // Cleanup: unbind the VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  void Renderer2D::updateAndSplitRenderList() {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerSystem);

    if (!quadVAO)
      initRenderData();

    const auto lastOpaqueCount = opaqueRenderers.size();
    lastOpaqueStaticCount = staticOpaqueRenderers.size();
    lastTransparentStaticCount = staticTransparentRenderers.size();

    if (!renderersToRemove.empty()) {
      std::erase_if(
        staticTransparentRenderers, [&](const auto &r) {
          return renderersToRemove.contains(r);
        }
      );
      std::erase_if(
        staticOpaqueRenderers, [&](const auto &r) {
          return renderersToRemove.contains(r);
        }
      );
      std::erase_if(
        transparentRenderers, [&](const auto &r) {
          return renderersToRemove.contains(r);
        }
      );
      std::erase_if(
        opaqueRenderers, [&](const auto &r) {
          return renderersToRemove.contains(r);
        }
      );
    }

    // Check if the invalid renderers are now valid
    if (!invalidRenderers.empty()) {
      // Sort out the invalid renderers
      auto valid_partition = std::ranges::partition(
        invalidRenderers, [](const auto &renderer) {
          return renderer && renderer->sprite && renderer->sprite->texture;
        }
      );
      renderersToAdd.insert(renderersToAdd.end(), invalidRenderers.begin(), valid_partition.begin());
      invalidRenderers.erase(invalidRenderers.begin(), valid_partition.begin());
    }

    // Add all pending renderers
    if (!renderersToAdd.empty()) {
      // Sort out the invalid renderers
      auto invalid_partition = std::ranges::partition(
        renderersToAdd, [](const auto &renderer) {
          return renderer->sprite && renderer->sprite->texture;
        }
      );
      invalidRenderers.insert(invalidRenderers.end(), invalid_partition.begin(), invalid_partition.end());
      renderersToAdd.erase(invalid_partition.begin(), invalid_partition.end());

      // First partition: static vs. non-static
      auto static_end = std::ranges::partition(
        renderersToAdd, [](const auto &renderer) {
          return renderer->Entity()->IsStatic();
        }
      );

      // Second partition within static: opaque vs. transparent
      auto static_opaque_end = std::ranges::partition(
        renderersToAdd.begin(), static_end.end(), [](const auto &renderer) {
          return !renderer->sprite->transparent;
        }
      );

      // Third partition within non-static: opaque vs. transparent
      auto opaque_end = std::ranges::partition(
        static_end.begin(), renderersToAdd.end(), [](const auto &renderer) {
          return !renderer->sprite->transparent;
        }
      );

      // Add all the renderers to their appropriate vectors
      staticOpaqueRenderers.insert(staticOpaqueRenderers.end(), renderersToAdd.begin(), static_opaque_end.begin());
      staticTransparentRenderers.insert(
        staticTransparentRenderers.end(), static_opaque_end.begin(), static_end.begin()
      );
      opaqueRenderers.insert(opaqueRenderers.end(), static_end.begin(), opaque_end.begin());
      transparentRenderers.insert(transparentRenderers.end(), opaque_end.begin(), opaque_end.end());
    }

    resortOpaque = lastOpaqueCount != opaqueRenderers.size();

    renderersToRemove.clear();
    renderersToAdd.clear();
    SceneManager::ActiveScene()->particleSystemRegistry.prerender();
  }

  void Renderer2D::opaquePass(const uint framebuffer) {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerSystem);

    const auto &particleSystemRegistry = SceneManager::ActiveScene()->particleSystemRegistry;
    if (opaqueRenderers.empty() && staticOpaqueRenderers.empty() && particleSystemRegistry.subrange.empty())
      return;

    zSort = false;

    // Render the static opaque sprites
    buildAndRenderStaticBatch(
      staticOpaqueRenderers, staticOpaqueBatchData, staticOpaqueFlushList,
      staticOpaqueRenderers.size() != lastOpaqueStaticCount, staticOpaqueBatchVBO, framebuffer
    );

    // Prepare the non-static opaque sprites
    std::vector<Renderable2D *> renderables;
    renderables.reserve(opaqueRenderers.size() + particleSystemRegistry.subrange.size());
    renderables.insert(renderables.end(), opaqueRenderers.begin(), opaqueRenderers.end());
    renderables.insert(
      renderables.end(), particleSystemRegistry.subrange.begin(), particleSystemRegistry.subrange.end()
    );

    const uint particleCount = std::transform_reduce(
      particleSystemRegistry.subrange.begin(), particleSystemRegistry.subrange.end(), 0u, std::plus<>(),
      [](const auto &ps) {
        return ps->capacity;
      }
    );

    buildAndRenderBatch(renderables, opaqueFlushList, particleCount, resortOpaque, framebuffer);
  }

  void Renderer2D::transparentPass(const uint framebuffer) {
    ENGINE_PROFILE_FUNCTION(ProfilingLevel::PerSystem);

    const auto &particleSystemRegistry = SceneManager::ActiveScene()->particleSystemRegistry;
    if (opaqueRenderers.empty() && staticOpaqueRenderers.empty() &&
        particleSystemRegistry.particleSystems.begin() == particleSystemRegistry.subrange.begin())
      return;

    zSort = true;

    // Render the static transparent sprites
    buildAndRenderStaticBatch(
      staticTransparentRenderers, staticTransparentBatchData, staticTransparentFlushList,
      staticTransparentRenderers.size() != lastTransparentStaticCount, staticTransparentBatchVBO, framebuffer
    );

    const auto transparentEnd = particleSystemRegistry.subrange.size() == 0
                                  ? particleSystemRegistry.particleSystems.end()
                                  : particleSystemRegistry.subrange.begin();

    // Prepare the non-static transparent sprites
    std::vector<Renderable2D *> renderables;
    renderables.reserve(
      transparentRenderers.size() + particleSystemRegistry.particleSystems.size() -
      particleSystemRegistry.subrange.size()
    );
    renderables.insert(renderables.end(), transparentRenderers.begin(), transparentRenderers.end());
    renderables.insert(renderables.end(), particleSystemRegistry.particleSystems.begin(), transparentEnd);

    const uint particleCount = std::transform_reduce(
      particleSystemRegistry.particleSystems.begin(), transparentEnd, 0u, std::plus<>(),
      [](const auto &ps) {
        return ps->capacity;
      }
    );

    buildAndRenderBatch(renderables, transparentFlushList, particleCount, true, framebuffer);
  }

  void Renderer2D::render(const uint framebuffer) {
    updateAndSplitRenderList();
    glBindVertexArray(quadVAO);
    opaquePass(framebuffer);
    transparentPass(framebuffer);
    glBindVertexArray(0);
  }

  void Renderer2D::addRenderer(SpriteRenderer *renderer) {
    scene->resources.renderables.push_back(renderer);
    renderersToAdd.push_back(renderer);
  }

  void Renderer2D::removeRenderer(SpriteRenderer *renderer) {
    std::erase(scene->resources.renderables, renderer);
    renderersToRemove.insert(renderer);
  }
}
